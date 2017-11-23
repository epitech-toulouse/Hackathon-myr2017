#include <iostream>
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
        this->_time = packet->time;
        this->_lat = packet->lat;
        this->_lon = packet->lon;
        this->_alt = packet->alt;
        this->_unit = packet->unit;
        this->_satUsed = packet->satUsed;
        this->_quality = packet->quality;
        this->_groundSpeed = packet->groundSpeed;
	}
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

double GPS::getGPSSpeed() const noexcept
{
    return _groundSpeed;
}
}