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
	bool is_connected() const noexcept;

private:
	void _read() noexcept;
	void _write() noexcept;
	std::chrono::milliseconds _now() const noexcept;

private:
	std::string _host;
	uint16_t _port;
	std::vector<ApiStereoCameraPacketPtr> _packets;
	int _socket;
	bool _connected;
	bool _running;
	std::thread _thread_read;
	std::thread _thread_write;
	std::chrono::milliseconds _latest_read;
};

}
