#pragma once

#include <atomic>
#include <chrono>
#include <string>
#include <thread>
#include <vector>
#include <cstring>
#include <cstdint>
#include <ApiCodec/ApiStereoCameraPacket.hpp>
#include "constants.hh"
#include "Gateway/Gateway.hh"
#include "Oz/Unit.hh"

namespace Oz {

class Camera : public Unit
{
	friend class CameraStateError;

public:
	explicit Camera(Gateway::Gateway & gateway);
	bool has_image() const noexcept;
	void update() override;
	const std::weak_ptr<uint8_t[]> get_left_image() const noexcept;
	const std::weak_ptr<uint8_t[]> get_right_image() const noexcept;

private:
	void _read() noexcept;
	void _update_buffers(const std::shared_ptr<ApiStereoCameraPacket> &, const uint8_t *, size_t);

private:
	Gateway::Gateway & _gateway;
	std::shared_ptr<uint8_t[]> _left_buffer;
	std::shared_ptr<uint8_t[]> _right_buffer;
	bool _has_image;
	std::chrono::milliseconds _latest_read;
	std::thread _thread;
};

}
