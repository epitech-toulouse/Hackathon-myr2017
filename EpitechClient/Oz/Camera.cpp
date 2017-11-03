#include <iostream>
#include <memory>
#include <ApiCodec/ApiStereoCameraPacket.hpp>
#include "Oz/Camera.hh"
#include "exceptions.hh"
#include "utilities.hh"

using std::chrono::duration_cast;

namespace Oz {

static const size_t CAMERA_RESOLUTION_X = 752;
static const size_t CAMERA_RESOLUTION_Y = 480;
static const size_t CAMERA_RESOLUTION = CAMERA_RESOLUTION_X * CAMERA_RESOLUTION_Y;
static const size_t BUFFER_SIZE = 0x400000;
static const std::chrono::milliseconds WAIT_TIME_MS (100);

ClientCamera::ClientCamera(Gateway::Gateway & gateway) :
	_gateway { gateway },
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

void ClientCamera::start()
{
	if (!_running) {
		_thread = std::thread(&ClientCamera::_read, this);
	}
}

void ClientCamera::stop() noexcept
{
	if (_running.load()) {
		_running.store(false);
		_thread.join();
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
	_running.store(true);
	while (_running.load()) {
		std::shared_ptr<ApiStereoCameraPacket> packet = _gateway.get<ApiStereoCameraPacket>();
		if (packet == nullptr) {
			std::this_thread::sleep_for(WAIT_TIME_MS);
			continue;
		}
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
