#include "Motor.hh"

namespace Oz {

    static const std::chrono::milliseconds WAIT_TIME_MS (1000);

Motor::Motor(Gateway::Gateway &gateway):
    _gateway { gateway },
    _speed { 0 },
    _angleSpeed { 10 },
    _angle { 0 }{
}

Motor::~Motor(){
}

void	Motor::update(){
        _gateway.emplace<HaMotorsPacket>(_speed + ((_angleSpeed/2) * (_angle/128)), _speed - ((_angleSpeed/2) * (_angle/128)));
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

}
