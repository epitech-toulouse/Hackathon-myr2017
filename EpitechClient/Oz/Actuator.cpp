#include <iostream>
#include <cstring>
#include "Oz/Actuator.hh"

namespace Oz {

Actuator::Actuator(Gateway::Gateway & gateway) :
	_gateway { gateway },
    _position { 0 }
{
}

void Actuator::update()
{
    _gateway.emplace<ApiMoveActuatorPacket>(_position);
}

uint8_t Actuator::getPosition() const noexcept
{
    return this->_position;
}

void Actuator::setPosition(uint8_t position)
{
    _position = position;
}
}