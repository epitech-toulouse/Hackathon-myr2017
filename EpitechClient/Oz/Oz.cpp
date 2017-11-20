#include <iostream>
#include "Oz/Oz.hh"

namespace Oz {

Oz::Oz(Gateway::Gateway & gateway) :
	camera { gateway },
	lidar { gateway },
	motor {},
	cpu {},
	_iterable { camera, lidar, motor, cpu }
{
}

const Camera & Oz::getCamera() const noexcept
{
	return this->camera;
}

const Lidar & Oz::getLidar() const noexcept
{
	return this->lidar;
}

const Motor & Oz::getMotor() const noexcept
{
	return this->motor;
}

const CPU & Oz::getCPU() const noexcept
{
	return this->cpu;
}

Oz::iterator Oz::begin() noexcept
{
	return _iterable.begin();
}

Oz::iterator Oz::end() noexcept
{
	return _iterable.end();
}

}
