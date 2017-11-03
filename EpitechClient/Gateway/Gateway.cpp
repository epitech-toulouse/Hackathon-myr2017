#include <ApiCodec/Naio01Codec.hpp>
#include <ApiCodec/ApiWatchdogPacket.hpp>
#include "Gateway.hh"
#include "exceptions.hh"

namespace Gateway
{

static const size_t BUFFER_SIZE = 0x400000;
static const std::chrono::milliseconds WAIT_TIME_MS (100);

Gateway::Gateway(const std::string & hostAddress, uint16_t hostMainPort, uint16_t hostCameraPort) :
	_running { false },
	_main_socket { hostAddress, hostMainPort },
	_camera_socket { hostAddress, hostCameraPort }
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

void Gateway::start()
{
	if (!_running.load()) {
		try {
			_running.store(true);
			_main_socket.connect();
			_camera_socket.connect();
			_read_thr = std::thread(&Gateway::_read, this);
			_write_thr = std::thread(&Gateway::_write, this);
		} catch (...) {
			_running.store(false);
			throw;
		}
	}
}

void Gateway::stop()
{
	if (_running.load()) {
		_running.store(false);
		_read_thr.join();
		_write_thr.join();
		_main_socket.disconnect();
		_camera_socket.disconnect();
	}
}

void Gateway::enqueue(std::unique_ptr<BaseNaio01Packet> && packet)
{
	_packet_queue.push(std::move(packet));
}

void Gateway::_read() noexcept
{
	try {
		std::unique_ptr<uint8_t[]> rx_buffer(new uint8_t[BUFFER_SIZE]);
		while (_running.load()) {
			this->_decodePackets(rx_buffer.get(), _main_socket);
			this->_decodePackets(rx_buffer.get(), _camera_socket);
			std::this_thread::sleep_for(WAIT_TIME_MS);
		}
	} catch (const ClientException & error) {
		std::cerr << error.what() << std::endl;
		_running.store(false);
	}
}

void Gateway::_write() noexcept
{
	try {
		while (_running.load()) {
			while (!_packet_queue.empty()) {
				std::unique_ptr<BaseNaio01Packet> packet = std::move(_packet_queue.front());
				_packet_queue.pop();
				std::unique_ptr<cl_copy::Buffer> buffer (packet->encode());
				_main_socket.write(buffer->data(), buffer->size());
			}
			ApiWatchdogPacket packet(0xFEU);
			std::unique_ptr<cl_copy::Buffer> buffer (packet.encode());
			_camera_socket.write(buffer->data(), buffer->size());
			std::this_thread::sleep_for(WAIT_TIME_MS);
		}
	} catch (const ClientException & error) {
		std::cerr << error.what() << std::endl;
		_running.store(false);
	}
}

void Gateway::_decodePackets(uint8_t * rx_buffer, const Socket & sock)
{
	Naio01Codec codec;
	size_t rx_bytes = sock.read(rx_buffer, BUFFER_SIZE);
	bool has_header_packet = false;
	if (rx_bytes > 0 && codec.decode(rx_buffer, static_cast<uint>(rx_bytes), has_header_packet)) {
		if (has_header_packet) {
			std::cerr << "Header packet ?" << std::endl;
		}
		for (auto && base_packet : codec.currentBasePacketList) {
			this->_set(std::move(base_packet));
		}
		codec.currentBasePacketList.clear();
	}
}


void Gateway::_set(std::shared_ptr<BaseNaio01Packet> && packet) noexcept
{
	_last_packets.emplace(std::type_index(typeid(*packet)), packet);
}

}
