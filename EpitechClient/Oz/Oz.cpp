#include <iostream>
#include "Oz/Oz.hh"

namespace Oz {

Oz::Oz(Gateway::Gateway & gateway) :
	camera { gateway },
	lidar { gateway },
	motor { gateway },
	cpu {},
	gps { gateway },
	gyro { gateway },
	odo { gateway },
	actuator { gateway},
	_iterable { camera, lidar, motor, cpu, gps, gyro, odo, actuator}
{
}

const Camera & Oz::getCamera() const noexcept
{
	return this->camera;
}

Camera & Oz::getCamera() noexcept
{
	return this->camera;
}

const Lidar & Oz::getLidar() const noexcept
{
	return this->lidar;
}

Lidar & Oz::getLidar() noexcept
{
	return this->lidar;
}

const Motor & Oz::getMotor() const noexcept
{
	return this->motor;
}

Motor & Oz::getMotor() noexcept
{
	return this->motor;
}

const CPU & Oz::getCPU() const noexcept
{
	return this->cpu;
}

CPU & Oz::getCPU() noexcept
{
	return this->cpu;
}

const GPS & Oz::getGPS() const noexcept
{
	return this->gps;
}

GPS & Oz::getGPS() noexcept
{
	return this->gps;
}

const Gyro & Oz::getGyro() const noexcept
{
	return this->gyro;
}

Gyro & Oz::getGyro() noexcept
{
	return this->gyro;
}

const ODO & Oz::getODO() const noexcept
{
	return this->odo;
}

ODO & Oz::getODO() noexcept
{
	return this->odo;
}

const Actuator & Oz::getActuator() const noexcept
{
	return this->actuator;
}

Actuator & Oz::getActuator() noexcept
{
	return this->actuator;
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
