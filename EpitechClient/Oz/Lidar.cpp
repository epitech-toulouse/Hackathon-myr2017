#include <iostream>
#include <cstring>
#include "Oz/Lidar.hh"

namespace Oz {

Lidar::Lidar(Gateway::Gateway & gateway) :
	_gateway { gateway },
	_distances { nullptr }
{
}

void Lidar::update()
{
	auto buffer = make_shared_array<uint16_t, LIDAR_CAPTURE_RESOLUTION>();
	std::shared_ptr<HaLidarPacket> packet = _gateway.get<HaLidarPacket>();
	if (packet != nullptr) {
		std::memcpy(buffer.get(), packet->distance, LIDAR_CAPTURE_RESOLUTION * sizeof(uint16_t));
		for (uint16_t & value : *buffer) {
			if (value < LIDAR_MIN_DISTANCE || value > LIDAR_MAX_DISTANCE) {
				value = 0;
			}
		}
		_distances.swap(buffer);
	}
}

weak_array_ptr<uint16_t, LIDAR_CAPTURE_RESOLUTION> Lidar::get_distances() const noexcept
{
	return _distances;
}

}
