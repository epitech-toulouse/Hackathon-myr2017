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
    		_motorSpeed = _motorSpeed + ACCELERATION;
    	else if (_speed < _motorSpeed - ACCELERATION)
    		_motorSpeed = _motorSpeed - ACCELERATION;
    	else
    		_motorSpeed = _speed;

    	// if (_angle > _motorAngle + ACCELERATION)
    	// 	_motorAngle = _motorAngle + ACCELERATION;
    	// else if (_angle < _motorAngle - ACCELERATION)
    	// 	_motorAngle = _motorAngle - ACCELERATION;
    	// else
    		_motorAngle = _angle;

    	// if (_angleSpeed > _motorAngleSpeed + ACCELERATION)
    	// 	_motorAngleSpeed = _motorAngleSpeed + ACCELERATION;
    	// else if (_angleSpeed < _motorAngleSpeed - ACCELERATION)
    	// 	_motorAngleSpeed = _motorAngleSpeed - ACCELERATION;
    	// else
    	// 	_motorAngleSpeed = _angleSpeed;

    }
    _gateway.emplace<HaMotorsPacket>(_motorSpeed + ((_motorSpeed/2) * (_motorAngle/128)), _motorSpeed - ((_motorSpeed/2) * (_motorAngle/128)));
}

void    Motor::setSpeed(int8_t speed){
    if (_angle > 69)
    {
        if (speed > 84)
            _speed = 84;
        else
            _speed = speed; 
    }
    else if (speed > 100)
        _speed = 100;
    else
        _speed = speed;
}

void    Motor::setAngle(int8_t angle){
    if (angle > 69 && _speed <= 84)
        _angle = angle;
    else if (angle > 69)
        _angle = 69;
    else
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

double  Motor::getMotorSpeed() const{
    return(_motorSpeed);
}

double  Motor::getMotorAngle() const{
    return(_motorAngle);
}

double  Motor::getMotorAngleSpeed() const{
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
