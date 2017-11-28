#pragma once
#include <cstdint>
#include <chrono>
#include <ApiCodec/HaOdoPacket.hpp>
#include <ctime>
#include <iostream>
#include "constants.hh"
#include "utilities.hh"
#include "Gateway/Gateway.hh"
#include "Oz/Unit.hh"

using std::chrono::duration_cast;

namespace Oz
{

class ODO : public Unit
{
	using Clock = std::chrono::steady_clock;

public:
	explicit ODO(Gateway::Gateway & gateway);
	void update() override;
	double getSpeedFR() const noexcept;
	double getSpeedRR() const noexcept;
	double getSpeedRL() const noexcept;
	double getSpeedFL() const noexcept;
	double getSpeed() const noexcept;
	double getDistance() const noexcept;

private:
	Gateway::Gateway & _gateway;
	uint8_t _frontRight;
	uint8_t _rearRight;
	uint8_t _rearLeft;
	uint8_t _frontLeft;
	std::time_t _time;
	std::chrono::time_point<Clock> _lastTimeFR;
	std::chrono::time_point<Clock> _lastTimeRR;
	std::chrono::time_point<Clock> _lastTimeRL;
	std::chrono::time_point<Clock> _lastTimeFL;
	double _speedFR;
	double _speedRR;
	double _speedRL;
	double _speedFL;
	double _distance;
};

}
