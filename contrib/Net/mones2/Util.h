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
    inline static dword swapLong(dword value)
    {
        return (value>>24)+((value>>8)&0xff00)+((value<<8)&0xff0000)+(value<<24);
    }

    inline static word swapShort(word value)
    {
        return (value>>8)+(value<<8);
    }

    inline static word get2byte(byte *buf, dword offset)
    {
        word a;
        a = buf[offset++] << 8;
        return a | buf[offset];
    }

    inline static dword ipAddressToDword(byte a, byte b, byte c, byte d)
    {
        return (d << 24) | (c << 16) | (b << 8) | a;
    }

    inline static MonAPI::CString ipAddressToCString(dword a)
    {
        char buf[64];
        sprintf(buf, "%d.%d.%d.%d", a & 0xff, (a >> 8) & 0xff, (a >> 16) & 0xff, (a >> 24) & 0xff);
        return MonAPI::CString(buf);
    }

    inline static word calcCheckSum(dword *data,int size)
    {
        union{
            unsigned long long u64;
            dword            u32[2];
            word             u16[4];
        }sum;

        dword tmp;


        sum.u64=0;
        for(;(dword)size>=sizeof(dword);size-=sizeof(dword))
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
