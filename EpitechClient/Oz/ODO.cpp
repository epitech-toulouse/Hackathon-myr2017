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
	_lastTimeFR { Clock::now() },
	_lastTimeRR { Clock::now() },
	_lastTimeRL { Clock::now() },
	_lastTimeFL { Clock::now() },
	_speedFR { 0 },
	_speedRR { 0 },
	_speedRL { 0 },
	_speedFL { 0 },
	_distance { 0.0 }
{
}

void ODO::update()
{
	using std::chrono::milliseconds;
	auto now = Clock::now();
	int check = 0;
	std::shared_ptr<HaOdoPacket> packet = _gateway.get<HaOdoPacket>();
	if (packet != nullptr) {
		if (this->_frontRight != packet->fr) {
			double dt = double(duration_cast<milliseconds>(now - _lastTimeFR).count()) / 3600.0;
			this->_speedFR = 6.465 / dt;
			this->_frontRight = packet->fr;
			this->_lastTimeFR = now;
			check += 1;
		}
		if (this->_rearRight != packet->rr) {
			double dt = double(duration_cast<milliseconds>(now - _lastTimeRR).count()) / 3600.0;
			this->_speedRR = 6.465 / dt;
			this->_rearRight = packet->rr;
			this->_lastTimeRR = now;
			check += 1;
		}
		if (this->_rearLeft != packet->rl) {
			double dt = double(duration_cast<milliseconds>(now - _lastTimeRL).count()) / 3600.0;
			this->_speedRL = 6.465 / dt;
			this->_rearLeft = packet->rl;
			this->_lastTimeRL = now;
			check += 1;
		}
		if (this->_frontLeft != packet->fl) {
			double dt = double(duration_cast<milliseconds>(now - _lastTimeFL).count()) / 3600.0;
			this->_speedFL = 6.465 / dt;
			this->_frontLeft = packet->fl;
			this->_lastTimeFL = now;
			check += 1;
		}
		this->_distance += 6.465;
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

double ODO::getDistance() const noexcept
{
	return(this->_distance);
}

}
