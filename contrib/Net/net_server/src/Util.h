#ifndef _MONA_MONES_UTIL_
#define _MONA_MONES_UTIL_

#include <sys/types.h>
#include <monapi.h>
#include <monapi/CString.h>
#include <monalibc.h>

namespace mones {

class Util
{
public:
    inline static uint32_t swapLong(uint32_t value)
    {
        return (value>>24)+((value>>8)&0xff00)+((value<<8)&0xff0000)+(value<<24);
    }

    inline static uint16_t swapShort(uint16_t value)
    {
        return (value>>8)+(value<<8);
    }

    inline static uint16_t get2uint8_t(uint8_t *buf, uint32_t offset)
    {
        uint16_t a;
        a = buf[offset++] << 8;
        return a | buf[offset];
    }

    inline static uint32_t ipAddressToDuint16_t(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
    {
        return (d << 24) | (c << 16) | (b << 8) | a;
    }

    inline static MonAPI::CString ipAddressToCString(uint32_t a)
    {
        char buf[64];
        sprintf(buf, "%d.%d.%d.%d", a & 0xff, (a >> 8) & 0xff, (a >> 16) & 0xff, (a >> 24) & 0xff);
        return MonAPI::CString(buf);
    }

    inline static uint16_t calcCheckSum(uint32_t *data,int size)
    {
        union{
            unsigned long long u64;
            uint32_t            u32[2];
            uint16_t             u16[4];
        }sum;

        uint32_t tmp;


        sum.u64=0;
        for(;(uint32_t)size>=sizeof(uint32_t);size-=sizeof(uint32_t))
            sum.u64+=*data++;
        if(size>0)sum.u64+=*data&((1<<(size*8))-1);

        tmp=sum.u32[1];
        sum.u32[1]=0;
        sum.u64+=tmp;
        tmp=sum.u32[1];
        sum.u32[1]=0;
        sum.u32[0]+=tmp;

        tmp=sum.u16[1];
        sum.u16[1]=0;
        sum.u32[0]+=tmp;
        tmp=sum.u16[1];
        sum.u16[1]=0;
        sum.u16[0]+=tmp;

        return ~sum.u16[0];
    }

};

}; // namespace mones
#endif
