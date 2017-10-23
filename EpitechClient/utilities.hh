#pragma once

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
