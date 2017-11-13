#include "Gateway.hh"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vector>

#include <vitals/CLBuffer.hpp>
#include <ApiCodec/ApiStereoCameraPacket.hpp>
#include <ApiCodec/ApiWatchdogPacket.hpp>
#include <ApiCodec/BaseNaio01Packet.hpp>
#include <ApiCodec/Naio01Codec.hpp>

#include "Socket.hh"
#include "constants.hh"
#include "exceptions.hh"

using std::dynamic_pointer_cast;

namespace Gateway
{

Gateway::Gateway(const std::string & host, const std::string & main_port, const std::string & camera_port) :
	_running { false },
	_main_socket { host, main_port, 16384 },
	_camera_socket { host, camera_port, 1048576 },
	_enable_camera { true },
	_stats {
		{ "command_packets_received", 0 },
		{ "command_packets_lost", 0 },
		{ "command_packets_bad", 0 },
		{ "camera_packets_received", 0 },
		{ "camera_packets_lost", 0 },
		{ "camera_packets_bad", 0 },
		{ "command_packets_transmitted", 0 },
		{ "camera_packets_transmitted", 0 }
	}
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

bool Gateway::is_command_connected() const noexcept
{
	return _main_socket.is_connected();
}

bool Gateway::is_camera_connected() const noexcept
{
	return _camera_socket.is_connected();
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
		_connect_condv.notify_all();
		_read_thr.join();
		_write_thr.join();
		this->_disconnect();
	}
}

void Gateway::enable_camera(bool enable) noexcept
{
	_enable_camera = enable;
}

const Gateway::Stats & Gateway::get_stats() const noexcept
{
	return _stats;
}

void Gateway::_connect() noexcept
{
	if (_connect_mutex.try_lock()) {
		this->_await_connect(_main_socket);
		if (_enable_camera) {
			this->_await_connect(_camera_socket);
		}
	} else {
		while (!_connect_mutex.try_lock()) {
			std::mutex cvw_mutex;
			std::unique_lock<std::mutex> cvw_lock(cvw_mutex);
			auto now = std::chrono::system_clock::now();
			if (_connect_condv.wait_until(cvw_lock, now + SOCKET_RECONNECT_DELAY) == std::cv_status::no_timeout) {
				_connect_mutex.unlock();
				return;
			}
		}
	}
	_connect_mutex.unlock();
}

void Gateway::_disconnect() noexcept
{
	if (_connect_mutex.try_lock()) {
		this->_ensure_disconnect(_main_socket);
		this->_ensure_disconnect(_camera_socket);
		std::this_thread::sleep_for(SOCKET_RECONNECT_DELAY);
	} else {
		while (!_connect_mutex.try_lock()) {
			std::mutex cvw_mutex;
			std::unique_lock<std::mutex> cvw_lock(cvw_mutex);
			auto now = std::chrono::system_clock::now();
			if (_connect_condv.wait_until(cvw_lock, now + SOCKET_RECONNECT_DELAY) == std::cv_status::no_timeout) {
				_connect_mutex.unlock();
				return;
			}
		}
	}
	_connect_mutex.unlock();
}

void Gateway::_reconnect() noexcept
{
	if (_connect_mutex.try_lock()) {
		this->_ensure_disconnect(_main_socket);
		this->_ensure_disconnect(_camera_socket);
		std::this_thread::sleep_for(SOCKET_RECONNECT_DELAY);
		this->_await_connect(_main_socket);
		if (_enable_camera) {
			this->_await_connect(_camera_socket);
		}
	} else {
		while (!_connect_mutex.try_lock()) {
			std::mutex cvw_mutex;
			std::unique_lock<std::mutex> cvw_lock(cvw_mutex);
			auto now = std::chrono::system_clock::now();
			if (_connect_condv.wait_until(cvw_lock, now + SOCKET_RECONNECT_DELAY) == std::cv_status::no_timeout) {
				_connect_mutex.unlock();
				return;
			}
		}
	}
	_connect_mutex.unlock();
}

void Gateway::_await_connect(Socket & sock) noexcept
{
	if (!_running || sock.is_connected()) {
		return;
	}
	std::clog << "Connecting to " << sock.get_host() << ":" << sock.get_port() << "... " << std::flush;
	while (!sock.is_connected()) {
		try {
			sock.connect();
		} catch (const SocketConnectError & error) {
			std::clog << "FAILED! (" << error.what() << ")" << std::endl;
			std::mutex cvw_mutex;
			std::unique_lock<std::mutex> cvw_lock(cvw_mutex);
			auto now = std::chrono::system_clock::now();
			if (_connect_condv.wait_until(cvw_lock, now + SOCKET_RECONNECT_DELAY) == std::cv_status::no_timeout) {
				return;
			}
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
			this->_decode_packets(codec, response, "command");
			response.clear();
			if (_enable_camera) {
				_camera_socket.read(response);
				this->_decode_packets(codec, response, "camera");
				response.clear();
			}
		} catch (const SocketPeerResetError & error) {
			std::cerr << error.what() << std::endl;
			this->_disconnect();
		} catch (const SocketReadError & error) {
			std::cerr << error.what() << std::endl;
		}
		std::this_thread::sleep_for(WAIT_TIME_MS);
		if (!this->is_connected()) {
			this->_reconnect();
		}
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
				_stats["command_packets_transmitted"] += 1;
			}
			if (_enable_camera) {
				ApiWatchdogPacket packet(0xFEU);
				std::unique_ptr<cl_copy::Buffer> buffer (packet.encode());
				_camera_socket.write({buffer->data(), buffer->size()});
				_stats["camera_packets_transmitted"] += 1;
			}
		} catch (const SocketPeerResetError & error) {
			std::cerr << error.what() << std::endl;
			this->_disconnect();
		} catch (const SocketWriteError & error) {
			std::cerr << error.what() << std::endl;
		}
		std::this_thread::sleep_for(WAIT_TIME_MS);
		if (!this->is_connected()) {
			this->_reconnect();
		}
	}
}

void Gateway::_decode_packets(Naio01Codec & codec, std::basic_string<uint8_t> & response, const std::string & name)
{
	bool has_header_packet = false;
	if (codec.decode(const_cast<uint8_t*>(response.data()), static_cast<unsigned>(response.size()), has_header_packet)) {
		_stats[name + "_packets_received"] += codec.currentBasePacketList.size();
		if (!has_header_packet) {
			_stats[name + "_packets_lost"] += codec.currentBasePacketList.size();
			/* std::cerr << "warning: Header packet not detected, dismissing data received." << std::endl; */
		}
		for (auto && base_packet : codec.currentBasePacketList) {
			this->_set(base_packet);
		}
		codec.currentBasePacketList.clear();
	} else {
		_stats[name + "_packets_bad"] += 1;
		/* std::cerr << "warning: Could not decode some packets." << std::endl; */
	}
}


void Gateway::_set(std::shared_ptr<BaseNaio01Packet> packet) noexcept
{
	_last_packets.emplace(std::type_index(typeid(*packet)), packet);
}

}
