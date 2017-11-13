#include "exceptions.hh"
#include "Oz/Camera.hh"
#include "Gateway/Socket.hh"

ClientException::ClientException(std::initializer_list<const char *> args)
{
	*text = 0;
	for (const char * argument : args) {
		strncat(text, argument, ClientException::bufsiz);
	}
}

const char * ClientException::what() const noexcept
{
	return text;
}


namespace Oz {

CameraStateError::CameraStateError(const Camera *, const char * msg) :
	ClientException { msg }
{ }

}

namespace Gateway
{

SocketException::SocketException(const Socket * parent, std::initializer_list<const char *> args) :
	ClientException(args),
	instance(parent)
{ }

SocketConnectError::SocketConnectError(const Socket * parent, const char * name, const char * msg) :
	SocketException(parent, { "Cannot connect to <", parent->_host.c_str(), ":", parent->_port.c_str(), ">: ", name, ": ", msg })
{ }

SocketReadError::SocketReadError(const Socket * parent, const char * name, const char * msg) :
	SocketException(parent, { "Cannot receive from <", parent->_host.c_str(), ":", parent->_port.c_str(), ">: ", name, ": ", msg })
{ }

SocketWriteError::SocketWriteError(const Socket * parent, const char * name, const char * msg) :
	SocketException(parent, { "Cannot send to <", parent->_host.c_str(), ":", parent->_port.c_str(), ">: ", name, ": ", msg })
{ }

SocketPeerResetError::SocketPeerResetError(const Socket * parent, const char * name, const char * msg) :
	SocketException(parent, { "Lost connection to <", parent->_host.c_str(), ":", parent->_port.c_str(), ">: ", name, ": ", msg })
{ }

SocketOperationalError::SocketOperationalError(const Socket * parent, const char * name, const char * msg) :
	SocketException(parent, { name, ": ", msg })
{ }

}
