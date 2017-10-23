#pragma once

#include <stdexcept>
#include "Oz/Camera.hh"

namespace Oz {

class ClientCamera; // Forward declaration

struct ClientCameraException : public std::runtime_error
{
	explicit ClientCameraException(const char * what_arg);
};

struct ClientCameraNetworkError : public ClientCameraException
{
	explicit ClientCameraNetworkError(ClientCamera * parent, const char * msg);
	std::string host;
	uint16_t port;
};

struct ClientCameraConnectError : public ClientCameraNetworkError
{
	explicit ClientCameraConnectError(ClientCamera * parent, const char * msg);
};

struct ClientCameraDisconnectError : public ClientCameraNetworkError
{
	explicit ClientCameraDisconnectError(ClientCamera * parent, const char * msg);
};

struct ClientCameraStateError : public ClientCameraException
{
	explicit ClientCameraStateError(ClientCamera * parent, const char * msg);
};

}
