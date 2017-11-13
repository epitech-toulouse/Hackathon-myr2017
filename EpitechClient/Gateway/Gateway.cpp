#include "Gateway.hh"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vector>

#include <vitals/CLBuffer.hpp>
#include <ApiCodec/ApiWatchdogPacket.hpp>
#include <ApiCodec/BaseNaio01Packet.hpp>
#include <ApiCodec/Naio01Codec.hpp>

#include "Socket.hh"
#include "constants.hh"
#include "exceptions.hh"

namespace Gateway
{

Gateway::Gateway(const std::string & host, const std::string & main_port, const std::string & camera_port) :
	_running { false },
	_main_socket { host, main_port },
	_camera_socket { host, camera_port, 65536 },
	_enable_camera { true }
{
}

Gateway::~Gateway()
{
	this->stop();
}

bool Gateway::is_running() const noexcept
{
	return _running.load();
}

bool Gateway::is_connected() const noexcept
{
	return _main_socket.is_connected() && (!_enable_camera || _camera_socket.is_connected());
}

void Gateway::start()
{
	if (!_running) {
		_running = true;
		_read_thr = this->_make_thread("read", &Gateway::_read);
		_write_thr = this->_make_thread("write", &Gateway::_write);
	}
}

void Gateway::stop()
{
	if (_running.load()) {
		_running.store(false);
		_read_thr.join();
		_write_thr.join();
		this->_disconnect();
	}
}

void Gateway::enable_camera(bool enable) noexcept
{
	_enable_camera = enable;
}

void Gateway::enqueue(std::unique_ptr<BaseNaio01Packet> && packet)
{
	_packet_queue.push(std::move(packet));
}

void Gateway::_connect() noexcept
{
	std::lock_guard<std::mutex> guard(_connect_mutex);
	this->_await_connect(_main_socket);
	if (_enable_camera) {
		this->_await_connect(_camera_socket);
	}
}

void Gateway::_disconnect() noexcept
{
	std::lock_guard<std::mutex> guard(_connect_mutex);
	this->_ensure_disconnect(_main_socket);
	this->_ensure_disconnect(_camera_socket);
}

void Gateway::_reconnect() noexcept
{
	std::lock_guard<std::mutex> guard(_connect_mutex);
	this->_ensure_disconnect(_main_socket);
	this->_ensure_disconnect(_camera_socket);
	this->_await_connect(_main_socket);
	if (_enable_camera) {
		this->_await_connect(_camera_socket);
	}
}

void Gateway::_await_connect(Socket & sock) noexcept
{
	if (sock.is_connected()) {
		return;
	}
	std::clog << "Connecting to " << sock.get_host() << ":" << sock.get_port() << "... " << std::flush;
	while (!sock.is_connected()) {
		try {
			sock.connect();
		} catch (const SocketConnectError & error) {
			std::clog << "FAILED! (" << error.what() << ")" << std::endl;
			std::this_thread::sleep_for(SOCKET_RECONNECT_DELAY);
			std::clog << "Reconnecting now... " << std::flush;
		}
		if (!_running) {
			std::clog << "STOP" << std::endl;
			return;
		}
	}
	std::clog << "OK" << std::endl;
}

void Gateway::_ensure_disconnect(Socket & sock) noexcept
{
	if (sock.is_connected()) {
		sock.disconnect();
	}
}

void Gateway::_read() noexcept
{
	this->_connect();
	Naio01Codec codec;
	std::basic_string<uint8_t> response;
	while (_running) {
		try {
			_main_socket.read(response);
			this->_decode_packets(codec, response);
			response.clear();
			if (_enable_camera) {
				_camera_socket.read(response);
				this->_decode_packets(codec, response);
				response.clear();
			}
		} catch (const SocketPeerResetError & error) {
			std::cerr << error.what() << std::endl;
			this->_reconnect();
		} catch (const SocketReadError & error) {
			std::cerr << error.what() << std::endl;
		}
		std::this_thread::sleep_for(WAIT_TIME_MS);
	}
}

void Gateway::_write() noexcept
{
	this->_connect();
	while (_running) {
		try {
			while (!_packet_queue.empty()) {
				std::unique_ptr<BaseNaio01Packet> packet = std::move(_packet_queue.front());
				_packet_queue.pop();
				std::unique_ptr<cl_copy::Buffer> buffer (packet->encode());
				_main_socket.write({buffer->data(), buffer->size()});
			}
			if (_enable_camera) {
				ApiWatchdogPacket packet(0xFEU);
				std::unique_ptr<cl_copy::Buffer> buffer (packet.encode());
				_camera_socket.write({buffer->data(), buffer->size()});
			}
		} catch (const SocketPeerResetError & error) {
			std::cerr << error.what() << std::endl;
			this->_reconnect();
		} catch (const SocketWriteError & error) {
			std::cerr << error.what() << std::endl;
		}
		std::this_thread::sleep_for(WAIT_TIME_MS);
	}
}

void Gateway::_decode_packets(Naio01Codec & codec, std::basic_string<uint8_t> & response)
{
	bool has_header_packet = false;
	if (codec.decode(const_cast<uint8_t*>(response.data()), static_cast<unsigned>(response.size()), has_header_packet)) {
		if (has_header_packet) {
			for (auto && base_packet : codec.currentBasePacketList) {
				this->_set(std::move(base_packet));
			}
		} else {
			std::cerr << "warning: Header packet not detected, dismissing data received." << std::endl;
		}
		codec.currentBasePacketList.clear();
	} else {
		std::cerr << "warning: Could not decode some packets." << std::endl;
	}
}


void Gateway::_set(std::shared_ptr<BaseNaio01Packet> && packet) noexcept
{
	_last_packets.emplace(std::type_index(typeid(*packet)), packet);
}

}
