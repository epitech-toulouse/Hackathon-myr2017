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
    std::shared_ptr<HaActuatorPacket> packet = _gateway.get<HaActuatorPacket>();
	if (packet != nullptr) {
        this->_position = packet->position;
    }
}

uint8_t Actuator::getPosition() const noexcept
{
    return this->_position;
}
}