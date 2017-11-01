#include <iostream>
#include <memory>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <ApiCodec/Naio01Codec.hpp>
#include <ApiCodec/ApiStereoCameraPacket.hpp>
#include <ApiCodec/ApiWatchdogPacket.hpp>
#include "Oz/Camera.hh"
#include "Oz/CameraException.hh"
#include "utilities.hh"

using std::dynamic_pointer_cast;
using std::chrono::duration_cast;

namespace Oz {

static const size_t CAMERA_RESOLUTION_X = 752;
static const size_t CAMERA_RESOLUTION_Y = 480;
static const size_t CAMERA_RESOLUTION = CAMERA_RESOLUTION_X * CAMERA_RESOLUTION_Y;
static const size_t BUFFER_SIZE = 0x400000;
static const std::chrono::milliseconds WAIT_TIME_MS (100);

ClientCamera::ClientCamera(Gateway &gateway) :
	_gateway { gateway },
	_packets { },
	_left_buffer { new uint8_t[CAMERA_RESOLUTION*4] },
	_right_buffer { new uint8_t[CAMERA_RESOLUTION*4] },
	_running { false }
{
}

ClientCamera::~ClientCamera()
{
	if (_left_buffer != nullptr) {
		delete[] _left_buffer;
		_left_buffer = nullptr;
	}
	if (_right_buffer != nullptr) {
		delete[] _right_buffer;
		_right_buffer = nullptr;
	}
}

void ClientCamera::run()
{
	if (!_gateway.getCameraSocket().isConnected()) {
		throw ClientCameraStateError(this, "Camera socket is not connected; cannot run.");
	}
	_thread_read = std::thread(&ClientCamera::_read, this);
	_thread_write = std::thread(&ClientCamera::_write, this);
	_running.store(true);
	while (_running.load()) {
		if (_packets.empty()) {
			std::this_thread::sleep_for(WAIT_TIME_MS);
			continue;
		}
		std::shared_ptr<ApiStereoCameraPacket> packet (_packets.back());
		_packets.clear();
		/* packet->dataBuffer->release_ownership(); */
		/* std::unique_ptr<uint8_t[]> capture_data (packet->dataBuffer->data()); */
		if (is_image_packet_zlib(packet)) {
			std::unique_ptr<uint8_t[]> data_buffer (new uint8_t[BUFFER_SIZE]);
			size_t data_size = zlib_uncompress(
				data_buffer.get(), packet->dataBuffer->data(),
				BUFFER_SIZE, packet->dataBuffer->size()
			);
			this->_update_buffers(packet, data_buffer.get(), data_size);
		} else {
			this->_update_buffers(packet, packet->dataBuffer->data(), packet->dataBuffer->size());
		}
		std::this_thread::sleep_for(WAIT_TIME_MS);
	}
}

void ClientCamera::stop() noexcept
{
	if (_running.load()) {
		_running.store(false);
		_thread_read.join();
		_thread_write.join();
	}
}

void ClientCamera::share_screen_buffers(const uint8_t ** left, const uint8_t ** right) const noexcept
{
	*left = _left_buffer;
	*right = _right_buffer;
}

bool ClientCamera::is_running() const noexcept
{
	return _running.load();
}

void ClientCamera::_read() noexcept
{
	Naio01Codec codec;
	std::unique_ptr<uint8_t[]> rx_buffer(new uint8_t[BUFFER_SIZE]);
	while (_running.load()) {
		ssize_t rx_bytes = read(_socket, rx_buffer.get(), BUFFER_SIZE);
		bool has_header_packet = false;
		if (rx_bytes > 0 && codec.decode(rx_buffer.get(), static_cast<uint>(rx_bytes), has_header_packet)) {
			if (has_header_packet) {
				std::cerr << "Header packet ?" << std::endl;
				continue;
			}
			for (auto && base_packet : codec.currentBasePacketList) {
				ApiStereoCameraPacketPtr packet = dynamic_pointer_cast<ApiStereoCameraPacket>(base_packet);
				if (packet.get() != nullptr) {
					_latest_read = this->_now();
					_packets.push_back(packet);
				} else {
					std::cerr << "Not a <ApiStereoCamera> packet ?! Type=0x"
						<< std::hex << unsigned(base_packet->getPacketId()) << std::dec
						<< " ?\?!" << std::endl;
				}
			}
			codec.currentBasePacketList.clear();
		}
		std::this_thread::sleep_for(WAIT_TIME_MS);
	}
}

void ClientCamera::_write() noexcept
{
	while (_running.load()) {
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

void ClientCamera::_update_buffers(
	const std::shared_ptr<ApiStereoCameraPacket> & packet,
	const uint8_t * data_buffer,
	size_t data_size)
{
	if (is_image_packet_bayer(packet)) {
		if (data_size < CAMERA_RESOLUTION * 2) {
			std::cerr << "Bad bayer stereo image size!" << std::endl;
			return;
		}
		for (size_t x = 0 ; x < CAMERA_RESOLUTION ; ++x) {
			_left_buffer[(x*4) + 0] = data_buffer[x];
			_left_buffer[(x*4) + 1] = data_buffer[x];
			_left_buffer[(x*4) + 2] = data_buffer[x];
			_left_buffer[(x*4) + 3] = 255;
		}
		for (size_t x = 0 ; x < CAMERA_RESOLUTION ; ++x) {
			_right_buffer[(x*4) + 0] = data_buffer[x + CAMERA_RESOLUTION];
			_right_buffer[(x*4) + 1] = data_buffer[x + CAMERA_RESOLUTION];
			_right_buffer[(x*4) + 2] = data_buffer[x + CAMERA_RESOLUTION];
			_right_buffer[(x*4) + 3] = 255;
		}
	} else {
		if (data_size < CAMERA_RESOLUTION * 3 * 2) {
			std::cerr << "Bad colorized image size!" << std::endl;
			return;
		}
		for (size_t x = 0 ; x < CAMERA_RESOLUTION ; ++x) {
			_left_buffer[(x*4) + 0] = data_buffer[(x*3) + 0];
			_left_buffer[(x*4) + 1] = data_buffer[(x*3) + 1];
			_left_buffer[(x*4) + 2] = data_buffer[(x*3) + 2];
			_left_buffer[(x*4) + 3] = 255;
		}
		for (size_t x = 0 ; x < CAMERA_RESOLUTION ; ++x) {
			_right_buffer[(x*4) + 0] = data_buffer[(x*3) + 0 + CAMERA_RESOLUTION];
			_right_buffer[(x*4) + 1] = data_buffer[(x*3) + 1 + CAMERA_RESOLUTION];
			_right_buffer[(x*4) + 2] = data_buffer[(x*3) + 2 + CAMERA_RESOLUTION];
			_right_buffer[(x*4) + 3] = 255;
		}
	}
}

}
