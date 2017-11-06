#pragma once

#include <iostream>
#include <atomic>
#include <thread>
#include <ApiCodec/ApiMotorsPacket.hpp>
#include "Gateway/Gateway.hh"

namespace Oz {

class Motor{
private:
	Gateway::Gateway & _gateway;
	std::atomic<bool> _running;
	std::thread _thread;
public:
	explicit Motor(Gateway::Gateway & gateway);
	~Motor();

	void start();
	void stop();
	void threadHandler();
};

}