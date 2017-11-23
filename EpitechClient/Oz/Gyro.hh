#pragma once
#include <cstdint>
#include <ApiCodec/HaGyroPacket.hpp>
#include "constants.hh"
#include "utilities.hh"
#include "Gateway/Gateway.hh"
#include "Oz/Unit.hh"

namespace Oz
{

class Gyro : public Unit
{
public:
	explicit Gyro(Gateway::Gateway & gateway);
	void update() override;
	int16_t getX() const noexcept;
    int16_t getY() const noexcept;
    int16_t getZ() const noexcept;

private:
	Gateway::Gateway & _gateway;
    int16_t _xAngle;
    int16_t _yAngle;
    int16_t _zAngle;
};

}