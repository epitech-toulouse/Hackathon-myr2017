#pragma once
#include <cstdint>
#include <ApiCodec/ApiMoveActuatorPacket.hpp>
#include <ctime>
#include <iostream>
#include "constants.hh"
#include "utilities.hh"
#include "Gateway/Gateway.hh"
#include "Oz/Unit.hh"

namespace Oz
{

class Actuator : public Unit
{
public:
	explicit Actuator(Gateway::Gateway & gateway);
	void update() override;
	uint8_t getPosition() const noexcept;
	void setPosition(uint8_t position);
	
private:
	Gateway::Gateway & _gateway;
    uint8_t _position;
};
}