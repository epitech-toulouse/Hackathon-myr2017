#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <typeindex>
#include <unordered_map>
#include <iostream>

#include <ApiCodec/BaseNaio01Packet.hpp>
#include <ApiCodec/Naio01Codec.hpp>

#include "Socket.hh"
#include "exceptions.hh"

using std::dynamic_pointer_cast;

namespace Gateway
{

class Gateway
{
	Gateway(const Gateway &) = delete;
	Gateway & operator= (const Gateway &) = delete;

public:
	explicit Gateway(const std::string & host, const std::string & main_port, const std::string & camera_port);
	~Gateway();
	bool is_running() const noexcept;
	bool is_connected() const noexcept;
	void start();
	void stop();
	void enable_camera(bool) noexcept;
	void enqueue(std::unique_ptr<BaseNaio01Packet> && packet);
	template<typename T> inline std::shared_ptr<T> get() const noexcept;

private:
	void _connect() noexcept;
	void _reconnect() noexcept;
	void _disconnect() noexcept;
	void _await_connect(Socket &) noexcept;
	void _ensure_disconnect(Socket &) noexcept;
	void _read() noexcept;
	void _write() noexcept;
	void _decode_packets(Naio01Codec & codec, std::basic_string<uint8_t> & response);
	void _set(std::shared_ptr<BaseNaio01Packet> && packet) noexcept;

// FIXME: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=80985
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnoexcept-type"
	template<typename Callable> inline std::thread _make_thread(const std::string &, Callable &&);
	template<typename Callable> void _thread_exception_wrapper(const std::string &, Callable &&);
#pragma GCC diagnostic pop

private:
	std::atomic<bool> _running;
	std::unordered_map<std::type_index, std::shared_ptr<BaseNaio01Packet>> _last_packets;
	std::queue<std::unique_ptr<BaseNaio01Packet>> _packet_queue;
	Socket _main_socket;
	Socket _camera_socket;
	std::thread _read_thr;
	std::thread _write_thr;
	bool _enable_camera;
	std::mutex _connect_mutex;
};

}

#include "Gateway.hpp"
