#include <cstdint>
#include <stdexcept>
#include <zlib.h>
#include <unistd.h>
#include <climits>
#include <cstring>
#include "utilities.hh"

/*
 * More C++ friendly zlib's uncompress() function.
 * Errors descriptions are from zlib man page for uncompress.
 */
size_t zlib_uncompress(uint8_t * dest, const uint8_t * src, long unsigned int dest_max_size, long unsigned int src_size)
{
	long unsigned int dest_size = dest_max_size;
	int status = uncompress(dest, &dest_size, src, src_size);
	switch (status) {
	case Z_OK:
		break;
	case Z_BUF_ERROR:
		throw std::runtime_error("zlib: The buffer dest was not large enough to hold the uncompressed data");
	case Z_MEM_ERROR:
		throw std::runtime_error("zlib: Insufficient memory");
	case Z_DATA_ERROR:
		throw std::runtime_error("zlib: The compressed data (referenced by source) was corrupted");
	default:
		throw std::runtime_error("zlib: Unexpected error");
	}
	return dest_size;
}

/*
 * Converts 32-bits Bayer GRBG images to 24-bits RGBA images usable by the SFML.
 * Parameters `xres` and `yres` must match the resolution of the destination image.
 */
void bayer_grbg32_to_rgba24(uint8_t * dst, const uint8_t * src, size_t xres, size_t yres)
{
	size_t di = 0; // destination index;
	size_t si = 0; // source index;
	for (size_t y = 0 ; y < yres ; ++y) {
		for (size_t x = 0 ; x < xres ; ++x) {
			di = (x + y * xres) * 4;
			si = (x + y * xres * 2) * 2;
			dst[di] = src[si + 1];
			dst[di + 1] = static_cast<uint8_t>((src[si] >> 1) + (src[si + xres * 2 + 1] >> 1));
			dst[di + 2] = src[si + xres * 2];
			dst[di + 3] = 255;
		}
	}
}

std::string getexe(void)
{
	char buffer[PATH_MAX];
	ssize_t len = readlink("/proc/self/exe", buffer, PATH_MAX);
	if (0 < len) {
		buffer[len] = 0;
		*strrchr(buffer, '/') = 0;
		return std::string(buffer);
	} else {
		return "";
	}
}
