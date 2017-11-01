#pragma once
	
#include <iostream>
#include <string>
#include <atomic>
#include <memory>

class Socket; // Forward declaration

class Gateway {
private:
	std::map[uint8_t, std::shared_ptr<BaseNaio01Packet>] _packets;
	Socket _mainSocket;
	Socket _cameraSocket;
public:
	Gateway(std::string hostAddress, uint16_t hostMainPort, uint16_t hostCameraPort);
	~Gateway();
	template <typename T>
	std::shared_ptr<T> _getPacket() {
		return _packets[T().getPacketId()];
	}
};

class Socket {
private:
	int _socket;
	std::atomic<bool> _connected;
	std::string _address;
	uint16_t _port;
public:
	Socket(std::string address, uint16_t port);
	~Socket();
	int getSocket();
	bool isConnected() const noexcept;
	void connect();
	void disconnect();
};