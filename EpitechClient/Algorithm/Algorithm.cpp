#include "Algorithm.hh"
#include "Oz/Oz.hh"
#include "Oz/Camera.hh"
#include "Oz/Motor.hh"

namespace Algorithm
{

Algorithm::Algorithm(Oz::Oz & oz) :
	_oz { oz }
{
}

void Algorithm::init()
{
	Oz::Camera & camera = _oz.getCamera();
	camera.enable_compression(false);
	camera.enable_raw(true);
}

void Algorithm::update()
{
	Oz::Motor & motor = _oz.getMotor();
	motor.setSpeed(10);
	motor.setAngle(90);
}

}
