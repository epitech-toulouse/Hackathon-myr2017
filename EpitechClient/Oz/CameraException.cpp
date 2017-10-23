#include "Oz/CameraException.hh"

namespace Oz {

ClientCameraException::ClientCameraException(const char * what_arg) :
	std::runtime_error(what_arg)
{ }

ClientCameraNetworkError::ClientCameraNetworkError(ClientCamera * parent, const char * msg) :
	ClientCameraException(msg),
	host(parent->_host),
	port(parent->_port)
{ }

ClientCameraConnectError::ClientCameraConnectError(ClientCamera * parent, const char * msg) :
	ClientCameraNetworkError(parent, msg)
{ }

ClientCameraDisconnectError::ClientCameraDisconnectError(ClientCamera * parent, const char * msg) :
	ClientCameraNetworkError(parent, msg)
{ }

ClientCameraStateError::ClientCameraStateError(ClientCamera *, const char * msg) :
	ClientCameraException(msg)
{ }

}
