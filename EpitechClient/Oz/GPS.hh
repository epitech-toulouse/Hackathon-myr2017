#pragma once
#include <cstdint>
#include <ApiCodec/HaGpsPacket.hpp>
#include "constants.hh"
#include "utilities.hh"
#include "Gateway/Gateway.hh"
#include "Oz/Unit.hh"

namespace Oz
{

class GPS : public Unit
{
public:
	explicit GPS(Gateway::Gateway & gateway);
	void update() override;
	double getLat() const noexcept;
    double getLon() const noexcept;
    double getAlt() const noexcept;

private:
	Gateway::Gateway & _gateway;
    uint64_t _time;                                                                                                                                                              
    double _lat;                                                                                                                                                                 
    double _lon;                                                                                                                                                                 
    double _alt;                                                                                                                                                                 
    uint8_t _unit;                                                                                                                                                               
    uint8_t _satUsed;                                                                                                                                                            
    uint8_t _quality;                                                                                                                                                            
    double _groundSpeed; 
};

}