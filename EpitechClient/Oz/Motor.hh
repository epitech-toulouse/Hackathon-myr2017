#pragma once

#include <iostream>
#include <thread>
#include <ctime>
#include <ApiCodec/HaMotorsPacket.hpp>
#include "Gateway/Gateway.hh"
#include "Oz/Unit.hh"

namespace Oz {

class Motor : public Unit {
private:
	Gateway::Gateway & _gateway;
	int8_t _speed;
	int8_t _angleSpeed;
	int8_t _angle;
	int8_t _motorSpeed;
	int8_t _motorAngleSpeed;
	int8_t _motorAngle;
	std::time_t _lastTime;

	std::thread _thread;

	static const int8_t ACCELERATION = 10;

public:
	explicit Motor(Gateway::Gateway & gateway);
	~Motor();

	void update() override;

	void setSpeed(int8_t speed);
	void setAngle(int8_t angle);
	void setAngleSpeed(int8_t speed);
	int8_t getSpeed() const;
	int8_t getAngle() const;
	int8_t getAngleSpeed() const;
	int8_t getMotorSpeed() const;
	int8_t getMotorAngle() const;
	int8_t getMotorAngleSpeed() const;
	void stop();
};

}
