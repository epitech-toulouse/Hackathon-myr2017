#include "Motor.hh"

namespace Oz {

    static const std::chrono::milliseconds WAIT_TIME_MS (1000);

Motor::Motor(Gateway::Gateway &gateway):
    _gateway { gateway },
    _running { false }{
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
        //void Gateway::enqueue(std::unique_ptr<BaseNaio01Packet> && packet)
        //std::unique_ptr<ApiMotorsPacket> toto(new ApiMotorsPacket(10, 10));
        _gateway.enqueue(std::make_unique<ApiMotorsPacket>(10, 10));
        std::this_thread::sleep_for(WAIT_TIME_MS);
    }
}
}
