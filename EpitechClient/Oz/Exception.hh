#pragma once

#include <stdexcept>
#include "Oz/Camera.hh"

namespace Oz {

class ClientCamera; // Forward declaration
class Socket; // Forward declaration

struct OzException : public std::runtime_error
{
	explicit OzException(const char * what_arg);
};

struct SocketNetworkError : public OzException
{
	explicit SocketNetworkError(Socket * parent, const char * msg);
	std::string host;
	uint16_t port;
};

struct ClientCameraStateError : public OzException
{
	explicit ClientCameraStateError(ClientCamera * parent, const char * msg);
};

struct SocketConnectError : public SocketNetworkError
{
	explicit SocketConnectError(Socket * parent, const char * msg);
};

struct SocketDisconnectError : public SocketNetworkError
{
	explicit SocketDisconnectError(Socket * parent, const char * msg);
};

}