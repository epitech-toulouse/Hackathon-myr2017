#pragma once

#include <atomic>
#include <chrono>
#include <string>
#include <thread>
#include <vector>
#include <cstring>
#include <cstdint>
#include <ApiCodec/ApiStereoCameraPacket.hpp>
#include "../Gateway/Gateway.hh"

namespace Oz {

class ClientCamera
{
	friend class ClientCameraStateError;

public:
	explicit ClientCamera(Gateway &gateway);
	~ClientCamera();
	void run();
	void stop() noexcept;
	void share_screen_buffers(const uint8_t **, const uint8_t **) const noexcept;
	bool is_running() const noexcept;

private:
	void _read() noexcept;
	void _write() noexcept;
	std::chrono::milliseconds _now() const noexcept;
	void _update_buffers(const std::shared_ptr<ApiStereoCameraPacket> &, const uint8_t *, size_t);

private:
	Gateway &_gateway;
	std::vector<ApiStereoCameraPacketPtr> _packets;
	uint8_t * _left_buffer;
	uint8_t * _right_buffer;
	std::atomic<bool> _running;
	std::thread _thread_read;
	std::thread _thread_write;
	std::chrono::milliseconds _latest_read;
};

}
