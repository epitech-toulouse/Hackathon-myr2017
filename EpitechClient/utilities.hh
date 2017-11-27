#pragma once

#include <memory>
#include <string>
#include <deque>
#include <chrono>
#include <vector>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <ApiCodec/ApiStereoCameraPacket.hpp>

/*
 * Says whether an ApiStereoCameraPacket contains zlib-compressed images.
 */
inline bool is_image_packet_zlib(const std::shared_ptr<ApiStereoCameraPacket> & packet)
{
	return (
		packet->imageType == ApiStereoCameraPacket::ImageType::RAW_IMAGES_ZLIB ||
        	packet->imageType == ApiStereoCameraPacket::ImageType::UNRECTIFIED_COLORIZED_IMAGES_ZLIB ||
        	packet->imageType == ApiStereoCameraPacket::ImageType::RECTIFIED_COLORIZED_IMAGES_ZLIB
	);
}
 
/*
 * Says whether an ApiStereoCameraPacket contains bayer (aka. raw) images.
 */
inline bool is_image_packet_bayer(const std::shared_ptr<ApiStereoCameraPacket> & packet)
{
	return (
		packet->imageType == ApiStereoCameraPacket::ImageType::RAW_IMAGES ||
		packet->imageType == ApiStereoCameraPacket::ImageType::RAW_IMAGES_ZLIB
	);
}

size_t zlib_uncompress(uint8_t * dest, const uint8_t * src, long unsigned int dest_max_size, long unsigned int src_size);
void bayer_grbg32_to_rgba24(uint8_t * dst, const uint8_t * src, size_t xres, size_t yres);
std::string getexe(void);

inline bool ends_with(std::string const & value, std::string const & suffix)
{
    if (suffix.size() <= value.size()) {
	    return std::equal(suffix.rbegin(), suffix.rend(), value.rbegin());
    } else {
	    return false;
    }
}

template<typename T, std::size_t N>
using shared_array_ptr = std::shared_ptr<std::array<T, N>>;

template<typename T, std::size_t N>
using weak_array_ptr = std::weak_ptr<std::array<T, N>>;

template<typename T, std::size_t N>
shared_array_ptr<T, N> make_shared_array(void)
{
	return std::make_shared<std::array<T, N>>();
}

bool is_sf_image_empty(const sf::Image &);

namespace math
{
template<typename T>
inline constexpr T pi(void) noexcept
{
	return static_cast<T>(3.14159265358979323846264338327950288);
}
}

template<class T>
sf::Vector2<T> sf_vec2_from_polar(T r, T theta)
{
	T phi = theta * math::pi<T>() / T{180};
	return { r * std::cos(phi), r * std::sin(phi) };
}

template<class Unit, class Callable>
Unit timed_call(Callable callable)
{
	using clock = std::chrono::steady_clock;
	std::chrono::time_point<clock> begin = clock::now();
	callable();
	return std::chrono::duration_cast<Unit>(clock::now() - begin);
}
