#include <iostream>
#include <cstring>
#include "Oz/Lidar.hh"

namespace Oz {

Lidar::Lidar(Gateway::Gateway & gateway) :
	_gateway { gateway },
	_distances { make_shared_array<uint16_t, LIDAR_CAPTURE_RESOLUTION>() },
	_detections { 0 },
	_rightmost_angle { 90 },
	_leftmost_angle { 90 }
{
}

void Lidar::update()
{
	auto buffer = make_shared_array<uint16_t, LIDAR_CAPTURE_RESOLUTION>();
	std::shared_ptr<HaLidarPacket> packet = _gateway.get<HaLidarPacket>();
	if (packet != nullptr) {
		std::memcpy(buffer.get(), packet->distance, LIDAR_CAPTURE_RESOLUTION * sizeof(uint16_t));
		_detections = 0;
		int16_t angle = -45;
		_rightmost_angle = 90;
		_leftmost_angle = 90;
		for (uint16_t & value : *buffer) {
			if (value < LIDAR_MIN_DISTANCE || value > LIDAR_MAX_DISTANCE) {
				value = 0;
			} else {
				if (angle < 90 && _rightmost_angle == 90) {
					_rightmost_angle = angle;
				}
				if (angle > 90) {
					_leftmost_angle = angle;
				}
				++_detections;
			}
			++angle;
		}
		_distances.swap(buffer);
	}
}

weak_array_ptr<uint16_t, LIDAR_CAPTURE_RESOLUTION> Lidar::get_distances() const noexcept
{
	return _distances;
}

uint16_t Lidar::detect() const noexcept
{
	return _detections;
}

uint16_t Lidar::detect_between(uint16_t begin, uint16_t end) const noexcept
{
	uint16_t local_detections = 0;
	for (uint16_t angle = begin ; angle <= end ; ++angle) {
		if ((*_distances)[angle] > 0) {
			++local_detections;
		}
	}
	return local_detections;
}

uint16_t Lidar::detect_right() const noexcept
{
	const uint16_t right = 45;
	const uint16_t opening = 45;
	return this->detect_between(right - opening / 2, right + opening / 2);
}

uint16_t Lidar::detect_left() const noexcept
{
	const uint16_t left = 45;
	const uint16_t opening = 45;
	return this->detect_between(left - opening / 2, left + opening / 2);
}

int16_t Lidar::detect_rightmost_angle() const noexcept
{
	return _rightmost_angle;
}

int16_t Lidar::detect_leftmost_angle() const noexcept
{
	return _leftmost_angle;
}

}
