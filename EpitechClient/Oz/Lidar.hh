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
	uint16_t detect() const noexcept;
	uint16_t detect_between(uint16_t begin, uint16_t end) const noexcept;
	uint16_t detect_right() const noexcept;
	uint16_t detect_left() const noexcept;
	int16_t detect_rightmost_angle() const noexcept;
	int16_t detect_leftmost_angle() const noexcept;

private:
	Gateway::Gateway & _gateway;
	shared_array_ptr<uint16_t, LIDAR_CAPTURE_RESOLUTION> _distances;
	uint16_t _detections;
	int16_t _rightmost_angle;
	int16_t _leftmost_angle;
};

}
