#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "Socket.hh"
#include "exceptions.hh"

namespace Gateway
{

Socket::Socket(const std::string & host, uint16_t port) :
	_fd { -1 },
	_connected { false },
	_host { host },
	_port { port }
{
}

Socket::~Socket()
{
	if (_connected.load()) {
		this->disconnect();
	}
}

int Socket::get_fd() const noexcept
{
	return _fd;
}

bool Socket::is_connected() const noexcept
{
	return _connected.load();
}

void Socket::connect()
{
	if (_connected.load()) {
		this->disconnect(); // FIXME: Should use a "reconnect" function for that.
	}
	struct sockaddr_in target;
	target.sin_addr.s_addr = ::inet_addr(_host.c_str());
	target.sin_family = AF_INET;
	target.sin_port = htons(_port);
	_fd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (0 > _fd) {
		throw SocketConnectError(this, strerror(errno));
	}
	if (0 > ::connect(_fd, (struct sockaddr *) &target, sizeof(target))) {
		throw SocketConnectError(this, strerror(errno));
	}
	_connected.store(true);
}

void Socket::disconnect()
{
	if (0 > ::shutdown(_fd, SHUT_RDWR)) {
		throw SocketDisconnectError(this, strerror(errno));
	}
	_host = "";
	_port = 0;
	_fd = -1;
	_connected.store(false);
}

size_t Socket::read(void * buffer, size_t bytes) const
{
	ssize_t bytes_received = ::read(_fd, buffer, bytes);
	if (bytes_received < 0) {
		throw SocketReadError(this, strerror(errno));
	}
	return static_cast<size_t>(bytes_received);
}

size_t Socket::write(void * buffer, size_t bytes) const
{
	ssize_t bytes_transfered = ::write(_fd, buffer, bytes);
	if (bytes_transfered < 0) {
		throw SocketWriteError(this, strerror(errno));
	}
	return static_cast<size_t>(bytes_transfered);
}

}
