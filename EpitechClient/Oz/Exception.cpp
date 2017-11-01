#include "Oz/CameraException.hh"

namespace Oz {

OzException::OzException(const char * what_arg) :
	std::runtime_error(what_arg)
{ }

SocketNetworkError::SocketNetworkError(Socket * parent, const char * msg) :
	OzException(msg),
	host(parent->_host),
	port(parent->_port)
{ }

SocketConnectError::SocketConnectError(Socket * parent, const char * msg) :
	SocketNetworkError(parent, msg)
{ }

SocketDisconnectError::SocketDisconnectError(Socket * parent, const char * msg) :
	SocketNetworkError(parent, msg)
{ }

ClientCameraStateError::ClientCameraStateError(ClientCamera *, const char * msg) :
	ClientCameraException(msg)
{ }

}
