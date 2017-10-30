#ifndef MOTOR_HH
#define MOTOR_HH
	#include <iostream>
	#include <thread>
	#include <mutex>

	#include "ApiCodec/Naio01Codec.hpp"

	class Motor{
	private:
		bool stopServerReadThreadAsked_;
		bool serverReadthreadStarted_;
		std::thread serverReadThread_;

		bool stopServerWriteThreadAsked_;
		bool serverWriteThreadStarted_;
		std::thread serverWriteThread_;

		int socket_desc_;
	public:
		Motor();
		~Motor();

		void server_read_thread();
		void server_write_thread();
	};
#endif