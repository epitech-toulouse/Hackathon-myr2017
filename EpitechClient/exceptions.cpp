#include "exceptions.hh"
#include "Oz/Camera.hh"
#include "Gateway/Socket.hh"

ClientException::ClientException(const char * what_arg) :
	std::runtime_error(what_arg)
{ }

namespace Oz {

ClientCameraStateError::ClientCameraStateError(const ClientCamera *, const char * msg) :
	ClientException(msg)
{ }

}

namespace Gateway
{

SocketException::SocketException(const Socket * parent, const char * msg) :
	ClientException(msg),
	host(parent->_host),
	port(parent->_port)
{ }

SocketConnectError::SocketConnectError(const Socket * parent, const char * msg) :
	SocketException(parent, msg)
{ }

SocketDisconnectError::SocketDisconnectError(const Socket * parent, const char * msg) :
	SocketException(parent, msg)
{ }

SocketReadError::SocketReadError(const Socket * parent, const char * msg) :
	SocketException(parent, msg)
{ }

SocketWriteError::SocketWriteError(const Socket * parent, const char * msg) :
	SocketException(parent, msg)
{ }

}
