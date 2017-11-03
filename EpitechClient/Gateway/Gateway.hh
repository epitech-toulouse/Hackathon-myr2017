#pragma once
	
#include <atomic>
#include <iostream>
#include <memory>
#include <queue>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <ApiCodec/BaseNaio01Packet.hpp>
#include "Socket.hh"

using std::dynamic_pointer_cast;

namespace Gateway
{

class Gateway
{
	Gateway(const Gateway &) = delete;
	Gateway & operator= (const Gateway &) = delete;

public:
	explicit Gateway(const std::string & hostAddress, uint16_t hostMainPort, uint16_t hostCameraPort);
	~Gateway();
	bool is_running() const noexcept;
	void start();
	void stop();
	void enqueue(std::unique_ptr<BaseNaio01Packet> && packet);
	template <typename T> inline std::shared_ptr<T> get() const noexcept;

private:
	void _read() noexcept;
	void _write() noexcept;
	void _decodePackets(uint8_t * rx_buffer, const Socket & sock);
	void _set(std::shared_ptr<BaseNaio01Packet> && packet) noexcept;

private:
	std::atomic<bool> _running;
	mutable std::unordered_map<std::type_index, std::shared_ptr<BaseNaio01Packet>> _last_packets;
	std::queue<std::unique_ptr<BaseNaio01Packet>> _packet_queue;
	Socket _main_socket;
	Socket _camera_socket;
	std::thread _read_thr;
	std::thread _write_thr;
};

template <typename T> inline std::shared_ptr<T> Gateway::get() const noexcept
{
	try {
		return dynamic_pointer_cast<T>(_last_packets.at(std::type_index(typeid(T))));
	} catch (const std::out_of_range &) {
		return nullptr;
	}
}

}
