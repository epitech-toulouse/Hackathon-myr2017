#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <ApiCodec/Naio01Codec.hpp>
#include <ApiCodec/ApiStereoCameraPacket.hpp>
#include <ApiCodec/ApiWatchdogPacket.hpp>
#include "Oz/Camera.hh"
#include "Oz/CameraException.hh"

using std::dynamic_pointer_cast;
using std::chrono::duration_cast;

namespace Oz {

static const size_t BUFFER_SIZE = 0x400000;
static const std::chrono::milliseconds WAIT_TIME_MS (100);

ClientCamera::ClientCamera(void) :
	_host { "" },
	_port { 0 },
	_packets { },
	_socket { -1 },
	_connected { false },
	_running { false }
{
}

ClientCamera::~ClientCamera()
{
	if (_connected) {
		this->disconnect();
	}
}

void ClientCamera::connect(std::string host, uint16_t port)
{
	if (_connected) {
		this->disconnect();
	}
	struct sockaddr_in target;
	_host = host;
	_port = port;
	target.sin_addr.s_addr = inet_addr(_host.c_str());
	target.sin_family = AF_INET;
	target.sin_port = htons(_port);
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (0 > _socket) {
		throw ClientCameraConnectError(this, strerror(errno));
	}
	if (0 > ::connect(_socket, (struct sockaddr *) &target, sizeof(target))) {
		throw ClientCameraConnectError(this, strerror(errno));
	}
	_connected = true;
}

void ClientCamera::disconnect()
{
	if (0 > shutdown(_socket, SHUT_RDWR)) {
		throw ClientCameraDisconnectError(this, strerror(errno));
	}
	_host = "";
	_port = 0;
	_socket = -1;
	_connected = false;
}

void ClientCamera::run()
{
	if (!_connected) {
		throw ClientCameraStateError(this, "Camera socket is not connected; cannot run.");
	}
	_running = true;
	_thread_read = std::thread(&ClientCamera::_read, this);
	_thread_write = std::thread(&ClientCamera::_write, this);
	while (_running) {
		// do stuff
		std::this_thread::sleep_for(WAIT_TIME_MS);
	}
}

void ClientCamera::stop() noexcept
{
	if (_running) {
		_running = false;
		_thread_read.join();
		_thread_write.join();
	}
}

bool ClientCamera::is_connected() const noexcept
{
	return _connected;
}

void ClientCamera::_read() noexcept
{
	Naio01Codec codec;
	uint8_t rx_buffer[BUFFER_SIZE];
	while (_running) {
		ssize_t rx_bytes = read(_socket, rx_buffer, BUFFER_SIZE);
		bool has_header_packet = false;
		if (rx_bytes > 0 && codec.decode(rx_buffer, static_cast<uint>(rx_bytes), has_header_packet)) {
			if (has_header_packet) {
				// TODO: Unexpected, do something, log.
			}
			for (auto && base_packet : codec.currentBasePacketList) {
				ApiStereoCameraPacketPtr packet = dynamic_pointer_cast<ApiStereoCameraPacket>(base_packet);
				if (packet.get() != nullptr) {
					_latest_read = this->_now();
					_packets.push_back(packet);
				} else {
					// TODO: Received a non-expected packet, log.
				}
			}
			codec.currentBasePacketList.clear();
		}
		std::this_thread::sleep_for(WAIT_TIME_MS);
	}
}

void ClientCamera::_write() noexcept
{
	while (_running) {
		ApiWatchdogPacketPtr packet = std::make_shared<ApiWatchdogPacket>(0xAC1D);
		cl_copy::BufferUPtr buffer = packet->encode();
		write(_socket, buffer->data(), buffer->size());
		std::this_thread::sleep_for(WAIT_TIME_MS);
	}
}

std::chrono::milliseconds ClientCamera::_now() const noexcept
{
	return duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
}

}
