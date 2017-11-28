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
	}
}

double Gyro::getX() const noexcept
{
	return _xAngle;
}

double Gyro::getY() const noexcept
{
    return _yAngle;
}

double Gyro::getZ() const noexcept
{
    return _zAngle;
}
}