#pragma once
#include <cstdint>
#include <ApiCodec/HaLidarPacket.hpp>
#include "constants.hh"
#include "utilities.hh"
#include "Gateway/Gateway.hh"
#include "Oz/Unit.hh"

namespace Oz
{

class Lidar : public Unit
{
public:
	explicit Lidar(Gateway::Gateway & gateway);
	void update() override;
	weak_array_ptr<uint16_t, LIDAR_CAPTURE_RESOLUTION> get_distances() const noexcept;

private:
	Gateway::Gateway & _gateway;
	shared_array_ptr<uint16_t, LIDAR_CAPTURE_RESOLUTION> _distances;
};

}
