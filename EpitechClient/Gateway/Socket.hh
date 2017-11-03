#pragma once

#include <atomic>
#include <chrono>
#include <string>
#include <thread>
#include <vector>
#include <cstring>
#include <cstdint>

namespace Gateway
{

class Socket
{
	friend class SocketException;

	Socket(const Socket &) = delete;
	Socket & operator=(const Socket &) = delete;

public:
	explicit Socket(const std::string & host, uint16_t port);
	~Socket();
	int get_fd() const noexcept;
	bool is_connected() const noexcept;
	void connect();
	void disconnect();
	size_t read(void * buffer, size_t bytes) const;
	size_t write(void * buffer, size_t bytes) const;

private:
	int _fd;
	std::atomic<bool> _connected; // TODO: Is atomic really necessary ?
	std::string _host;
	uint16_t _port;
};

}
