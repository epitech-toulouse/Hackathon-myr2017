#pragma once

#include <stdexcept>

/**
 * Base exception class for all EpitechClient's exceptions.
 * Shall never be raised itself, use descendents.
 */
struct ClientException : public std::runtime_error
{
	explicit ClientException(const char * what_arg);
};


namespace Oz
{

class ClientCamera; // Forward declaration

struct ClientCameraStateError : public ClientException
{
	explicit ClientCameraStateError(const ClientCamera * parent, const char * msg);
};

} /* namespace Oz */


namespace Gateway
{

class Socket; // Forward declaration

struct SocketException: public ClientException
{
	explicit SocketException(const Socket * parent, const char * msg);
	std::string host;
	uint16_t port;
};

struct SocketConnectError : public SocketException
{
	explicit SocketConnectError(const Socket * parent, const char * msg);
};

struct SocketDisconnectError : public SocketException
{
	explicit SocketDisconnectError(const Socket * parent, const char * msg);
};

struct SocketReadError : public SocketException
{
	explicit SocketReadError(const Socket * parent, const char * msg);
};

struct SocketWriteError : public SocketException
{
	explicit SocketWriteError(const Socket * parent, const char * msg);
};

} /* namespace Gateway */
