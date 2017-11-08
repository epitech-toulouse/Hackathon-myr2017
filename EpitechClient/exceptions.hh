#pragma once

#include <stdexcept>
#include <initializer_list>

/**
 * Base exception class for all EpitechClient's exceptions.
 * Shall never be raised itself, use descendents.
 */
struct ClientException : public std::exception
{
	static const size_t bufsiz = 1024;
	char text[bufsiz];
	ClientException(std::initializer_list<const char *> args);
	virtual const char * what() const noexcept;
};


namespace Oz
{

class Camera; // Forward declaration

struct CameraStateError : public ClientException
{
	explicit CameraStateError(const Camera * parent, const char * msg);
};

} /* namespace Oz */


namespace Gateway
{

class Socket; // Forward declaration

struct SocketException : public ClientException
{
	SocketException(const Socket * parent, std::initializer_list<const char *> args);
	const Socket * instance;
};

struct SocketConnectError : public SocketException
{
	explicit SocketConnectError(const Socket * parent, const char * name, const char * msg);
};

struct SocketReadError : public SocketException
{
	explicit SocketReadError(const Socket * parent, const char * name, const char * msg);
};

struct SocketWriteError : public SocketException
{
	explicit SocketWriteError(const Socket * parent, const char * name, const char * msg);
};

struct SocketPeerResetError : public SocketException
{
	explicit SocketPeerResetError(const Socket * parent, const char * name, const char * msg);
};

struct SocketOperationalError : public SocketException
{
	explicit SocketOperationalError(const Socket * parent, const char * name, const char * msg);
};

} /* namespace Gateway */
