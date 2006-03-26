#include "endian_util.h"
// 2006/03/25 by Yume : endian for MacOSX
union Swap64
{
	uint64_t origin;
	uint32_t dword[2];
};

uint16_t bswap16(uint16_t x)
{
	return (((x << 8) & 0xFF00) | ((x >> 8) & 0x00FF));
}

uint32_t bswap32(uint32_t x)
{
	return ((((x & 0x00FF) << 24) | ((x & 0xFF00) << 8) | ((x >> 8) & 0xFF00) | (x >> 24) & 0x00FF));
}

uint64_t bswap64(uint64_t x)
{
	union Swap64 tmp, ret;
	tmp.origin = x;
	ret.dword[0] = bswap32(tmp.dword[0]);
	ret.dword[1] = bswap32(tmp.dword[1]);
	return ret.origin;
}

uint16_t big2host16(uint16_t x)
{
#if defined(__BIG_ENDIAN__)
	return x;
#else
	return bswap16(x);
#endif
}

uint32_t big2host32(uint32_t x)
{
#if defined(__BIG_ENDIAN__)
	return x;
#else
	return bswap32(x);
#endif
}

uint64_t big2host64(uint32_t x)
{
#if defined(__BIG_ENDIAN__)
	return x;
#else
	return bswap64(x);
#endif
}

uint16_t host2big16(uint16_t x)
{
#if defined(__BIG_ENDIAN__)
	return x;
#else
	return bswap16(x);
#endif
}

uint32_t host2big32(uint32_t x)
{
#if defined(__BIG_ENDIAN__)
	return x;
#else
	return bswap32(x);
#endif
}

uint64_t host2big64(uint64_t x)
{
#if defined(__BIG_ENDIAN__)
	return x;
#else
	return bswap64(x);
#endif
}

uint16_t little2host16(uint16_t x)
{
#if defined(__LITTLE_ENDIAN__)
	return x;
#else
	return bswap16(x);
#endif
}

uint32_t little2host32(uint32_t x)
{
#if defined(__LITTLE_ENDIAN__)
        return x;
#else
        return bswap32(x);
#endif
}

uint64_t little2host64(uint64_t x)
{
#if defined(__LITTLE_ENDIAN__)
        return x;
#else
        return bswap64(x);
#endif
}

uint16_t host2little16(uint16_t x)
{
#if defined(__LITTLE_ENDIAN__)
        return x;
#else
        return bswap16(x);
#endif
}

uint32_t host2little32(uint32_t x)
{
#if defined(__LITTLE_ENDIAN__)
	return x;
#else
	return bswap32(x);
#endif
}

uint64_t host2little64(uint64_t x)
{
#if defined(__LITTLE_ENDIAN__)
        return x;
#else
        return bswap64(x);
#endif
}
