#pragma once
#include <cstdint>
#include <ApiCodec/HaOdoPacket.hpp>
#include <ctime>
#include <iostream>
#include "constants.hh"
#include "utilities.hh"
#include "Gateway/Gateway.hh"
#include "Oz/Unit.hh"

namespace Oz
{

class ODO : public Unit
{
public:
	explicit ODO(Gateway::Gateway & gateway);
	void update() override;
    double getSpeedFR() const noexcept;
    double getSpeedRR() const noexcept;
    double getSpeedRL() const noexcept;
    double getSpeedFL() const noexcept;
    double getSpeed() const noexcept;

private:
	Gateway::Gateway & _gateway;
    uint8_t _frontRight;
    uint8_t _rearRight;
    uint8_t _rearLeft;
    uint8_t _frontLeft;
    std::time_t _time;
    std::time_t _lastTimeFR;
    std::time_t _lastTimeRR;
    std::time_t _lastTimeRL;
    std::time_t _lastTimeFL;
    double _speedFR;
    double _speedRR;
    double _speedRL;
    double _speedFL;
};
}