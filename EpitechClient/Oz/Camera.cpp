#include <iostream>
#include <memory>
#include <ApiCodec/ApiCommandPacket.hpp>
#include <ApiCodec/ApiStereoCameraPacket.hpp>
#include "Oz/Camera.hh"
#include "exceptions.hh"
#include "utilities.hh"

using std::chrono::duration_cast;

namespace Oz {

Camera::Camera(Gateway::Gateway & gateway) :
	_gateway { gateway },
	_left_buffer { new uint8_t[CAMERA_CAPTURE_RESOLUTION * 4] },
	_right_buffer { new uint8_t[CAMERA_CAPTURE_RESOLUTION * 4] },
	_has_image { false }
{
}

const std::weak_ptr<uint8_t[]> Camera::get_left_image() const noexcept
{
	return _left_buffer;
}

const std::weak_ptr<uint8_t[]> Camera::get_right_image() const noexcept
{
	return _right_buffer;
}

bool Camera::has_image() const noexcept
{
	return _has_image;
}

void Camera::enable_compression(bool enable) noexcept
{
	using CommandType = ApiCommandPacket::CommandType;
	_gateway.emplace<ApiCommandPacket>(enable?
		CommandType::TURN_ON_IMAGE_ZLIB_COMPRESSION :
		CommandType::TURN_OFF_IMAGE_ZLIB_COMPRESSION
	);
}

void Camera::enable_raw(bool enable) noexcept
{
	using CommandType = ApiCommandPacket::CommandType;
	_gateway.emplace<ApiCommandPacket>(enable?
		CommandType::TURN_ON_API_RAW_STEREO_CAMERA_PACKET :
		CommandType::TURN_OFF_API_RAW_STEREO_CAMERA_PACKET
	);
}

void Camera::update()
{
	std::shared_ptr<ApiStereoCameraPacket> packet = _gateway.get<ApiStereoCameraPacket>();
	if (packet == nullptr) {
		if (!_gateway.is_camera_connected()) {
			_has_image = false;
		}
		return;
	}
	_has_image = true;
	if (is_image_packet_zlib(packet)) {
		std::unique_ptr<uint8_t[]> data_buffer (new uint8_t[CAMERA_BUFFER_SIZE]);
		size_t data_size = zlib_uncompress(
			data_buffer.get(), packet->dataBuffer->data(),
			CAMERA_BUFFER_SIZE, packet->dataBuffer->size()
		);
		this->_update_buffers(packet, data_buffer.get(), data_size);
	} else {
		this->_update_buffers(packet, packet->dataBuffer->data(), packet->dataBuffer->size());
	}
}

/**
 * Color disposition look likes this:
 * Row 0: {G,R,G,R,repeating ...} [for all EVEN rows]
 * Row 1: {B,G,B,G,repeating ...} [for all ODD rows]
 * Row ...: {repeating ...}
 */
void Camera::_update_buffers(
	const std::shared_ptr<ApiStereoCameraPacket> & packet,
	const uint8_t * data_buffer,
	size_t data_size)
{
	if (is_image_packet_bayer(packet)) {
		if (data_size < CAMERA_CAPTURE_RESOLUTION * 8) {
			std::cerr << "Bad bayer stereo image size!" << std::endl;
			return;
		}
		bayer_grbg32_to_rgba24(_left_buffer.get(), data_buffer, CAMERA_CAPTURE_WIDTH, CAMERA_CAPTURE_HEIGHT);
		bayer_grbg32_to_rgba24(_right_buffer.get(), data_buffer + CAMERA_CAPTURE_RESOLUTION * 4, CAMERA_CAPTURE_WIDTH, CAMERA_CAPTURE_HEIGHT);
	} else {
		if (data_size < CAMERA_CAPTURE_RESOLUTION * 3 * 2) {
			std::cerr << "Bad colorized image size!" << std::endl;
			return;
		}
	/*
	 * FIXME: GCC implicitly convert sometimes size_t to ptrdiff_t, and
	 * sometimes ptrdiff_t to size_t so it always triggers a warning here ><.
	 */
#if defined(__GNUC__)
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wsign-conversion"
#endif
		for (size_t x = 0 ; x < CAMERA_CAPTURE_RESOLUTION * 4 ; ++x) {
			_left_buffer[x*4+0] = data_buffer[x*3+0];
			_left_buffer[x*4+1] = data_buffer[x*3+1];
			_left_buffer[x*4+2] = data_buffer[x*3+2];
			_left_buffer[x*4+3] = 255;
		}
		for (size_t x = 0 ; x < CAMERA_CAPTURE_RESOLUTION * 4 ; ++x) {
			_right_buffer[x*4+0] = data_buffer[x*3+0+CAMERA_CAPTURE_RESOLUTION*4];
			_right_buffer[x*4+1] = data_buffer[x*3+1+CAMERA_CAPTURE_RESOLUTION*4];
			_right_buffer[x*4+2] = data_buffer[x*3+2+CAMERA_CAPTURE_RESOLUTION*4];
			_right_buffer[x*4+3] = 255;
		}
#if defined(__GNUC__)
# pragma GCC diagnostic pop
#endif
	}
}

}
