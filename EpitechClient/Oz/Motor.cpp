#include "Motor.hh"

namespace Oz {

    static const std::chrono::milliseconds WAIT_TIME_MS (1000);

Motor::Motor(Gateway::Gateway &gateway):
    _gateway { gateway },
    _speed { 0 },
    _angleSpeed { 0 },
    _angle { 0 },
    _motorSpeed { 0 },
    _motorAngleSpeed { 0 },
    _motorAngle { 0 },
    _lastTime { std::time(nullptr) }
{
    std::asctime(std::localtime(&_lastTime));
}

Motor::~Motor(){
}

void	Motor::update(){
	std::time_t actual = std::time(nullptr);
    std::asctime(std::localtime(&_lastTime));

    if (actual != _lastTime){
    	_lastTime = actual;
    	
    	if (_speed > _motorSpeed + ACCELERATION)
    		_motorSpeed = static_cast<int8_t> (_motorSpeed + ACCELERATION);
    	else if (_speed < _motorSpeed - ACCELERATION)
    		_motorSpeed = static_cast<int8_t> (_motorSpeed - ACCELERATION);
    	else
    		_motorSpeed = _speed;

    	if (_angle > _motorAngle + ACCELERATION)
    		_motorAngle = static_cast<int8_t> (_motorAngle + ACCELERATION);
    	else if (_angle < _motorAngle - ACCELERATION)
    		_motorAngle = static_cast<int8_t> (_motorAngle - ACCELERATION);
    	else
    		_motorAngle = _angle;

    	if (_angleSpeed > _motorAngleSpeed + ACCELERATION)
    		_motorAngleSpeed = static_cast<int8_t> (_motorAngleSpeed + ACCELERATION);
    	else if (_angleSpeed < _motorAngleSpeed - ACCELERATION)
    		_motorAngleSpeed = static_cast<int8_t> (_motorAngleSpeed - ACCELERATION);
    	else
    		_motorAngleSpeed = _angleSpeed;

    }
    _gateway.emplace<HaMotorsPacket>(_motorSpeed + ((_motorAngleSpeed/2) * (_motorAngle/128)), _motorSpeed - ((_motorAngleSpeed/2) * (_motorAngle/128)));
}

void    Motor::setSpeed(int8_t speed){
    _speed = speed;
}

void    Motor::setAngle(int8_t angle){
    _angle = angle;
}

void	Motor::setAngleSpeed(int8_t speed){
	_angleSpeed = speed;
}

int8_t  Motor::getSpeed() const{
    return(_speed);
}

int8_t  Motor::getAngle() const{
    return(_angle);
}

int8_t  Motor::getAngleSpeed() const{
    return(_angleSpeed);
}

int8_t  Motor::getMotorSpeed() const{
    return(_motorSpeed);
}

int8_t  Motor::getMotorAngle() const{
    return(_motorAngle);
}

int8_t  Motor::getMotorAngleSpeed() const{
    return(_motorAngleSpeed);
}

void	Motor::stop(){
	_speed = 0;
	_angle = 0;
	_angleSpeed = 0;
	_motorSpeed = 0;
	_motorAngle = 0;
	_motorAngleSpeed = 0;
}

}
