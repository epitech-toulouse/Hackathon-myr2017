#pragma once

namespace Gateway
{

template<typename T>
void Gateway::enqueue(std::unique_ptr<T>&& packet)
{
	_packet_queue.push(std::forward<std::unique_ptr<T>&&>(packet));
}

template<typename T, typename... Args>
void Gateway::emplace(Args&&... args)
{
	_packet_queue.push(std::make_unique<T>(std::forward<Args>(args)...));
}

template<typename T>
inline std::shared_ptr<T> Gateway::get() const noexcept
{
	try {
		return dynamic_pointer_cast<T>(_last_packets.at(std::type_index(typeid(T))));
	} catch (const std::out_of_range &) {
		return nullptr;
	}
}

// FIXME: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=80985
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnoexcept-type"

template<typename Callable>
inline std::thread Gateway::_make_thread(const std::string & name, Callable && f)
{
	return std::thread(&Gateway::_thread_exception_wrapper<Callable>, this, name, std::forward<Callable>(f));
}

template<typename Callable>
void Gateway::_thread_exception_wrapper(const std::string & name, Callable && invoke)
{
	try {
		(this->*invoke)();
	} catch (const ClientException & error) {
		std::cerr << "Thread " << name << " failed: " << error.what() << std::endl;
		_running = false;
	}
}

#pragma GCC diagnostic pop

}
