#pragma once

#include <chrono>
#include <string>
#include <thread>
#include <vector>
#include <cstring>
#include <cstdint>
#include <ApiCodec/ApiStereoCameraPacket.hpp>

namespace Oz {

class ClientCamera
{
	friend class ClientCameraNetworkError;
	friend class ClientCameraConnectError;
	friend class ClientCameraDisconnectError;
	friend class ClientCameraStateError;

public:
	explicit ClientCamera(void);
	~ClientCamera();
	void connect(std::string host, uint16_t port);
	void disconnect();
	void run();
	void stop() noexcept;
	void share_screen_buffers(const uint8_t **, const uint8_t **) const noexcept;
	bool is_connected() const noexcept;

private:
	void _read() noexcept;
	void _write() noexcept;
	std::chrono::milliseconds _now() const noexcept;
	void _update_buffers(const std::shared_ptr<ApiStereoCameraPacket> &, const uint8_t *, size_t);

private:
	std::string _host;
	uint16_t _port;
	std::vector<ApiStereoCameraPacketPtr> _packets;
	uint8_t * _left_buffer;
	uint8_t * _right_buffer;
	int _socket;
	bool _connected;
	bool _running;
	std::thread _thread_read;
	std::thread _thread_write;
	std::chrono::milliseconds _latest_read;
};

}
