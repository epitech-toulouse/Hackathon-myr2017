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
    double a = _motorSpeed + ((_motorSpeed/2) * (_motorAngle/128));
    double b = _motorSpeed - ((_motorSpeed/2) * (_motorAngle/128));
    _gateway.emplace<HaMotorsPacket>(a, b);
    // _gateway.emplace<HaMotorsPacket>((a > 127)? 127 : (a < -127 ? -127 : a), (b > 127)? 127 : (b < -127 ? -127 : b));
    //std::cout << (_motorSpeed + ((_motorSpeed/2) * (_motorAngle/128))) << " " << (_motorSpeed - ((_motorSpeed/2) * (_motorAngle/128))) << " " << _speed << " " << _angle << std::endl;
}

void    Motor::setSpeed(int8_t speed){
    if (std::abs(_angle) > 0 && std::abs(speed) > 84)
        _speed = (speed < 0 ? -84 : 84);
    else if (std::abs(_angle) < 61 && std::abs(speed) > 100)
        _speed = (speed < 0 ? -100 : 100);
    else
        _speed = speed;
}

void    Motor::setAngle(int8_t angle){
    if (std::abs(angle) > 61 && std::abs(_speed) > 84)
        _speed = (_speed < 0 ? -84 : 84);
    else if (std::abs(_speed) > 100)
        _speed = (_speed < 0 ? -100 : 100);
    _angle = _angle;
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
