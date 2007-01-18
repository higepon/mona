// 2006/03/25 by Yume : endian for MacOSX
#ifndef _ENDIAN__H_
#include <stdint.h>

uint16_t bswap16(uint16_t x);
uint32_t bswap32(uint32_t x);
uint64_t bswap64(uint64_t x);

uint16_t big2host16(uint16_t x);
uint32_t big2host32(uint32_t x);
uint64_t big2host64(uint64_t x);

uint16_t little2host16(uint16_t x);
uint32_t little2host32(uint32_t x);
uint64_t little2host64(uint64_t x);

uint16_t host2big16(uint16_t x);
uint32_t host2big32(uint32_t x);
uint64_t host2big64(uint64_t x);

uint16_t host2little16(uint16_t x);
uint32_t host2little32(uint32_t x);
uint64_t host2little64(uint64_t x);
#endif
