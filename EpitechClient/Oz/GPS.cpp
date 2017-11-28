#include <iostream>
#include <cstdio>
#include <cstring>
#include "Oz/GPS.hh"

namespace Oz {

GPS::GPS(Gateway::Gateway & gateway) :
	_gateway { gateway },
	 _time { 0 },
	 _lat { 0 },
	 _lon { 0 },
	 _alt { 0 },
	 _unit { 0 },
	 _satUsed { 0 },
	 _quality { 0 },
	 _groundSpeed { 0 }
{
}

void GPS::update()
{
	std::shared_ptr<HaGpsPacket> packet = _gateway.get<HaGpsPacket>();
	if (packet != nullptr) {
		_time = std::chrono::milliseconds(packet->time);
		_lat = packet->lat;
		_lon = packet->lon;
		_alt = packet->alt;
		_unit = static_cast<char>(packet->unit);
		_satUsed = packet->satUsed;
		_quality = packet->quality;
		_groundSpeed = packet->groundSpeed;
	}
}

std::chrono::milliseconds GPS::getTime() const noexcept
{
	return _time;
}

double GPS::getLon() const noexcept
{
	return _lon;
}

double GPS::getLat() const noexcept
{
	return _lat;
}

double GPS::getAlt() const noexcept
{
	return _alt;
}

char GPS::getUnit() const noexcept
{
	return _unit;
}

uint8_t GPS::getSatelliteCount() const noexcept
{
	return _satUsed;
}

uint8_t GPS::getQuality() const noexcept
{
	return _quality;
}

double GPS::getGroundSpeed() const noexcept
{
	return _groundSpeed;
}

}
