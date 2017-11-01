#include "Gateway.hh"

Gateway::Gateway(std::string hostAddress, uint16_t hostMainPort, uint16_t hostCameraPort):
_mainSocket { hostAddress, hostMainPort },
_cameraSocket { hostAddress, hostCameraPort } {
	std::cout << "I'm a gateway" << std::endl;
}

Gateway::~Gateway() {
	std::cout << "Gateway died" << std::endl;
}

void Gateway::_read() noexcept
{
	std::unique_ptr<uint8_t[]> rx_buffer(new uint8_t[BUFFER_SIZE]);
	while (_running.load()) {
		this->_decodePackets(rx_buffer, _mainSocket);
		this->_decodePackets(rx_buffer, _cameraSocket);
		std::this_thread::sleep_for(WAIT_TIME_MS);
	}
}

void Gateway::_decodePackets(std::unique_ptr<uint8_t> const & rx_buffer, Socket const & sock)
{
	Naio01Codec codec;
	ssize_t rx_bytes = read(sock.getSocket(), rx_buffer.get(), BUFFER_SIZE);
	bool has_header_packet = false;
	if (rx_bytes > 0 && codec.decode(rx_buffer.get(), static_cast<uint>(rx_bytes), has_header_packet)) {
		if (has_header_packet) {
			std::cerr << "Header packet ?" << std::endl;
			continue;
		}
		for (auto && base_packet : codec.currentBasePacketList) {
			this->_setPacket(base_packet);
		}
		codec.currentBasePacketList.clear();
	}
}

void Gateway::_write() noexcept
{
	while (_running.load()) {
		ApiWatchdogPacketPtr packet = std::make_shared<ApiWatchdogPacket>(0xAC1D);
		cl_copy::BufferUPtr buffer = packet->encode();
		write(_socket, buffer->data(), buffer->size());
		std::this_thread::sleep_for(WAIT_TIME_MS);
	}
}

void Gateway::_setPacket(std::shared_ptr<BaseNaio01Packet> packet)
{
	_packets[packet->getPacketId()] = packet;
}


Socket::Socket(std::string address, uint16_t port):
_socket { -1 },
_connected { false },
_address { address },
_port { port },
{

}

Socket::~Socket(){

}

int Socket::getSocket(){
	return _socket;
}

bool Socket::isConnected() const noexcept {
	return _connected;
}

void Socket::connect(){
	if (_connected.load()) {
		this->disconnect();
	}
	struct sockaddr_in target;
	_address = host;
	_port = port;
	target.sin_addr.s_addr = inet_addr(_address.c_str());
	target.sin_family = AF_INET;
	target.sin_port = htons(_port);
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (0 > _socket) {
		throw SocketConnectError(this, strerror(errno));
	}
	if (0 > ::connect(_socket, (struct sockaddr *) &target, sizeof(target))) {
		throw SocketConnectError(this, strerror(errno));
	}
	_connected.store(true);
}

void Socket::disconnect(){
	if (0 > shutdown(_socket, SHUT_RDWR)) {
		throw SocketDisconnectError(this, strerror(errno));
	}
	_address = "";
	_port = 0;
	_socket = -1;
	_connected.store(false);
}