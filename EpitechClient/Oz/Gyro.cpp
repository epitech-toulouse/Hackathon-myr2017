#include <iostream>
#include <cstring>
#include "Oz/Gyro.hh"

namespace Oz {

Gyro::Gyro(Gateway::Gateway & gateway) :
	_gateway { gateway },
    _xAngle { 0 },                                                                                                                                                     
    _yAngle { 0 },                                                                                                                                                     
    _zAngle { 0 }
{
}

void Gyro::update()
{
	std::shared_ptr<HaGyroPacket> packet = _gateway.get<HaGyroPacket>();
	if (packet != nullptr) {
        this->_xAngle = packet->x;
        this->_yAngle = packet->y;
        this->_zAngle = packet->z;
        std::cout << (int)packet->x << " " << (int)packet->y << " " << (int)packet->z << std::endl;
	}
}

int16_t Gyro::getX() const noexcept
{
	return _xAngle;
}

int16_t Gyro::getY() const noexcept
{
    return _yAngle;
}

int16_t Gyro::getZ() const noexcept
{
    return _zAngle;
}
}