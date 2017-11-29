#include <iostream>
#include <cstring>
#include "Oz/Gyro.hh"

namespace Oz {

Gyro::Gyro(Gateway::Gateway & gateway) :
	_gateway { gateway },
    _xAngle { 0 },                                                                                                                                                     
    _yAngle { 0 },                                                                                                                                                     
    _zAngle { 0 },
    _x { 0 },
    _y { 0 },
    _z { 0 }
{
}

void Gyro::update()
{
	std::shared_ptr<HaGyroPacket> packet = _gateway.get<HaGyroPacket>();
	if (packet != nullptr) {
        this->_xAngle = packet->x * (30.5 / 1000.0);
        this->_yAngle = packet->y * (30.5 / 1000.0);
        this->_zAngle = packet->z * (30.5 / 1000.0);
        _x += _xAngle;
        _y += _yAngle;
        _z += _zAngle;
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

double Gyro::getXX() const noexcept
{
    return _x;
}

double Gyro::getYY() const noexcept
{
    return _y;
}

double Gyro::getZZ() const noexcept
{
    return _z;
}

void Gyro::reset(){
    _x = 0;
    _y = 0;
    _z = 0;    
}
}