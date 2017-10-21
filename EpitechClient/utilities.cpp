#include <cstdint>
#include <stdexcept>
#include <zlib.h>
#include "utilities.hh"

/*
 * More C++ friendly zlib's uncompress() function.
 * Errors descriptions are from zlib man page for uncompress.
 */
size_t zlib_uncompress(uint8_t * dest, const uint8_t * src, size_t dest_max_size, size_t src_size)
{
	size_t dest_size = dest_max_size;
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
