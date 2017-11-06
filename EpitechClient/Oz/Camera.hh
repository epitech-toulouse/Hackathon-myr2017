#pragma once

#include <atomic>
#include <chrono>
#include <string>
#include <thread>
#include <vector>
#include <cstring>
#include <cstdint>
#include <ApiCodec/ApiStereoCameraPacket.hpp>
#include "Gateway/Gateway.hh"
#include "constants.hh"

namespace Oz {

class Camera
{
	friend class CameraStateError;

public:
	explicit Camera(Gateway::Gateway & gateway);
	~Camera();
	void start();
	void stop() noexcept;
	void share_screen_buffers(const uint8_t **, const uint8_t **) const noexcept;
	bool is_running() const noexcept;

private:
	void _read() noexcept;
	void _update_buffers(const std::shared_ptr<ApiStereoCameraPacket> &, const uint8_t *, size_t);

private:
	Gateway::Gateway & _gateway;
	uint8_t * _left_buffer;
	uint8_t * _right_buffer;
	std::atomic<bool> _running;
	std::chrono::milliseconds _latest_read;
	std::thread _thread;
};

}
