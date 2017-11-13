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
	std::atomic<int8_t> _speed;
	std::atomic<int8_t> _angleSpeed;
	std::atomic<int8_t> _angle;
	std::thread _thread;
public:
	explicit Motor(Gateway::Gateway & gateway);
	~Motor();

	void start();
	void stop();
	void threadHandler();

	void setSpeed(int8_t speed);
	void setAngle(int8_t angle);
	int8_t getSpeed() const;
	int8_t getAngle() const;
};

}
