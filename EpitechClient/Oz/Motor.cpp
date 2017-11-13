#include "Motor.hh"

namespace Oz {

    static const std::chrono::milliseconds WAIT_TIME_MS (1000);

Motor::Motor(Gateway::Gateway &gateway):
    _gateway { gateway },
    _running { false },
    _speed { 0 },
    _angleSpeed { 10 },
    _angle { 0 }{
}

Motor::~Motor(){
}

void    Motor::start(){
    if (!_running)
        _thread = std::thread(&Motor::threadHandler, this);
}

void    Motor::stop(){
    if (_running){
        _running = false;
        _thread.join();
    }
}

void	Motor::threadHandler(){
    _running = true;
    while (_running){
        _gateway.enqueue(std::make_unique<ApiMotorsPacket>(_speed + ((_angleSpeed/2) * (_angle/128)), _speed - ((_angleSpeed/2) * (_angle/128))));
        std::this_thread::sleep_for(WAIT_TIME_MS);
    }
}

void    Motor::setSpeed(int8_t speed){
    _speed = speed;
}

void    Motor::setAngle(int8_t angle){
    _angle = angle;
}

int8_t  Motor::getSpeed() const{
    return(_speed);
}

int8_t  Motor::getAngle() const{
    return(_angle);
}

}
