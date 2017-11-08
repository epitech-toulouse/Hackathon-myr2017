#pragma once

#include <mutex>
#include <string>
#include <vector>
#include <cstdint>
#include <iosfwd>

namespace Gateway
{

class Socket
{
	Socket(const Socket &) = delete;
	Socket & operator=(const Socket &) = delete;

public:
	friend struct SocketException;
	friend struct SocketConnectError;
	friend struct SocketReadError;
	friend struct SocketWriteError;
	friend struct SocketPeerResetError;
	friend struct SocketOperationalError;
	friend std::ostream & operator<<(std::ostream &, const Gateway::Socket &);

public:
	explicit Socket(const std::string & host, const std::string & port, size_t buffer_size = 8192);
	~Socket();
	const std::string & get_host() const noexcept;
	const std::string & get_port() const noexcept;
	std::string get_authority() const noexcept;
	bool is_connected() const noexcept;
	void connect();
	void disconnect() noexcept;
	void force_close() noexcept;
	void read(std::basic_string<uint8_t> & response);
	void write(const std::basic_string<uint8_t> & request);

private:
	size_t _buffer_recv();
	void _nonblock(bool);

private:
	int _fd;
	bool _connected;
	int _options;
	bool _nonblock_option;
	std::string _host;
	std::string _port;
	std::vector<uint8_t> _buffer;
	std::mutex _lock;
};

}
