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
	double getX() const noexcept;
    double getY() const noexcept;
    double getZ() const noexcept;
	double getXX() const noexcept;
    double getYY() const noexcept;
    double getZZ() const noexcept;
    void reset();

private:
	Gateway::Gateway & _gateway;
    double _xAngle;
    double _yAngle;
    double _zAngle;
    double _x;
    double _y;
    double _z;
};

}