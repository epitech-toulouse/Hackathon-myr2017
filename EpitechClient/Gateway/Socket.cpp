#include "Socket.hh"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <new>

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include "exceptions.hh"

namespace Gateway
{

Socket::Socket(const std::string & host, const std::string & port, size_t buffer_size) :
	_fd { -1 },
	_connected { false },
	_host { host },
	_port { port },
	_buffer { }
{
	_buffer.reserve(buffer_size);
}

Socket::~Socket()
{
	if (_connected) {
		this->disconnect();
	}
}

const std::string & Socket::get_host() const noexcept
{
	return _host;
}

const std::string & Socket::get_port() const noexcept
{
	return _port;
}

std::string Socket::get_authority() const noexcept
{
	return std::string(_host) + ":" + _port;
}

bool Socket::is_connected() const noexcept
{
	return _connected;
}

void Socket::connect()
{
	if (_connected) {
		this->disconnect();
	}
<<<<<<< HEAD
	struct sockaddr_in target;
	target.sin_addr.s_addr = ::inet_addr(_host.c_str());
	target.sin_family = AF_INET;
	target.sin_port = htons(_port);
	_fd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (0 > _fd) {
		throw SocketConnectError(this, strerror(errno));
=======
	struct addrinfo hints;
	struct addrinfo * results = nullptr;
	std::memset(&hints, 0, sizeof(hints));
	_fd = -1;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;
	const int gai_err = ::getaddrinfo(_host.c_str(), _port.c_str(), &hints, &results);
	if (gai_err != 0) {
		throw SocketConnectError(this, "getaddrinfo", gai_strerror(gai_err));
	}
	for (auto * addr = results ; addr != nullptr ; addr = addr->ai_next) {
		const int fd = ::socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
		if (0 > fd) {
			std::cerr << "warning: socket: " << strerror(errno) << std::endl;
			continue;
		}
		if (0 > ::connect(fd, addr->ai_addr, addr->ai_addrlen)) {
			/* std::cerr << "warning: connect: " << strerror(errno) << std::endl; */
			::close(fd);
			continue;
		}
		_fd = fd;
		break;
>>>>>>> 437273d70176d43e2e39df55b8292fc6b2d78322
	}
	::freeaddrinfo(results);
	if (0 > _fd) {
		throw SocketConnectError(this, "connect", strerror(errno));
	}
	_connected = true;
	_options = ::fcntl(_fd, F_GETFL);
}

void Socket::disconnect() noexcept
{
	_connected = false;
	if (0 > ::shutdown(_fd, SHUT_RDWR)) {
		std::cerr << "warning: shutdown<" << std::string(this->get_authority()) << ">: "
			<< strerror(errno) << std::endl;
		this->force_close();
	}
	_fd = -1;
}

void Socket::force_close() noexcept
{
	_connected = false;
	std::cerr << "warning: Force close attempted on socket connected to " << std::string(this->get_authority()) << std::endl;
	if (0 > ::close(_fd)) {
		std::cerr << "warning: close<" << std::string(this->get_authority()) << ">: " << strerror(errno) << std::endl;
	}
}

// NOTE: To read a fixed-size of bytes, just recv(2) in blocking-mode.
void Socket::read(std::basic_string<uint8_t> & response)
{
	size_t received = 0;
	if (_nonblock_option) {
		this->_nonblock(false);
	}
	received = this->_buffer_recv();
	response.append(std::basic_string<uint8_t>(_buffer.data(), received));
	{
		std::lock_guard<std::mutex> lock (_lock);
		this->_nonblock(true);
		while (received > 0) {
			try {
				received = this->_buffer_recv();
			} catch (const SocketReadError & error) {
				std::cerr << error.what() << std::endl;
				break;
			}
			if (received > 0) {
				response.append(std::basic_string<uint8_t>(_buffer.data(), received));
			}
		}
		this->_nonblock(false);
	}
}

void Socket::write(const std::basic_string<uint8_t> & request)
{
	std::lock_guard<std::mutex> lock (_lock);
	if (_nonblock_option) {
		this->_nonblock(false);
	}
	if (0 > ::send(_fd, request.data(), request.size(), MSG_NOSIGNAL)) {
		if (errno == ENOMEM) {
			throw std::bad_alloc();
		}
		if (errno == EPIPE || errno == ECONNRESET || errno == EBADF) {
			this->force_close();
			throw SocketPeerResetError(this, "send", strerror(errno));
		}
		throw SocketWriteError(this, "send", strerror(errno));
	}
}

size_t Socket::_buffer_recv()
{
	_buffer.clear();
	const ssize_t received = ::recv(_fd, _buffer.data(), _buffer.capacity(), 0);
	if (errno == ENOMEM) {
		throw std::bad_alloc();
	}
	if (received == 0 || errno == ECONNRESET || errno == EBADF) {
		this->force_close();
		throw SocketPeerResetError(this, "recv", strerror(errno));
	}
	if (received < 0) {
		if (errno == EWOULDBLOCK || errno == EAGAIN) {
			return 0;
		}
		throw SocketReadError(this, "recv", strerror(errno));
	}
	return static_cast<size_t>(received);
}

void Socket::_nonblock(bool enable)
{
	int options = enable? (_options | O_NONBLOCK) : (_options & (~O_NONBLOCK));
	if (0 > ::fcntl(_fd, F_SETFL, options)) {
		throw SocketOperationalError(this, "fcntl", strerror(errno));
	}
	_nonblock_option = enable;
}

std::ostream & operator<<(std::ostream & out, const Gateway::Socket & rhs)
{
	out << "Socket { "
		<< "<" << rhs._host << ":" << rhs._port << ">, "
		<< "<" << rhs._buffer.size() << " bytes>, "
		<< std::string(rhs._connected? "ON" : "OFF")
		<< " }";
	return out;
}

}
