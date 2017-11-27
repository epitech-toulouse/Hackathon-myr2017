#include <iostream>
#include <cstring>
#include "Oz/ODO.hh"

namespace Oz {

ODO::ODO(Gateway::Gateway & gateway) :
	_gateway { gateway },
    _frontRight { 0 },
    _rearRight { 0 },
    _rearLeft { 0 },
    _frontLeft { 0 },
    _lastTimeFR { std::time(nullptr) },
    _lastTimeRR { std::time(nullptr) },
    _lastTimeRL { std::time(nullptr) },
    _lastTimeFL { std::time(nullptr) },
    _speedFR { 0 },
    _speedRR { 0 },
    _speedRL { 0 },
    _speedFL { 0 }
{
}

void ODO::update()
{
    std::time_t time = std::time(nullptr);
    std::asctime(std::localtime(&time));
    std::shared_ptr<HaOdoPacket> packet = _gateway.get<HaOdoPacket>();
	if (packet != nullptr) {
        if (this->_frontRight != packet->fr) {
            this->_speedFR = 6.465 / static_cast<double>(time - this->_lastTimeFR) *  3.6 / 100;
            this->_frontRight = packet->fr;
            this->_lastTimeFR = time;
        }
        if (this->_rearRight != packet->rr) {
            this->_speedRR = 6.465 / static_cast<double>(time - this->_lastTimeRR) *  3.6 / 100;
            this->_rearRight = packet->rr;
            this->_lastTimeRR = time;
        }
        if (this->_rearLeft != packet->rl) {
            this->_speedRL = 6.465 / static_cast<double>(time - this->_lastTimeRL) *  3.6 / 100;
            this->_rearLeft = packet->rl;
            this->_lastTimeRL = time;
        }
        if (this->_frontLeft != packet->fl) {
            this->_speedFL = 6.465 / static_cast<double>(time - this->_lastTimeFL) *  3.6 / 100;
            this->_frontLeft = packet->fl;
            this->_lastTimeFL = time;
        }
    }
}

double ODO::getSpeedFR() const noexcept
{
    return this->_speedFR;
}
double ODO::getSpeedRR() const noexcept
{
    return this->_speedRR;
}
double ODO::getSpeedRL() const noexcept
{
    return this->_speedRL;
}
double ODO::getSpeedFL() const noexcept
{
    return this->_speedFL;
}
double ODO::getSpeed() const noexcept
{
    return (this->_speedFR + this->_speedRR + this->_speedRL + this->_speedFL) / 4;
}

}
