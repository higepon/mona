/*
 * Util.h - 
 *
 *   Copyright (c) 2009  Higepon(Taro Minowa)  <higepon@users.sourceforge.jp>
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  $Id: Util.h 261 2008-07-25 06:16:44Z higepon $
 */

#ifndef _UTIL_
#define _UTIL_

class Util
{
public:
    inline static uint16_t* ipAddressToU16Array(uint16_t* buf, uint32_t address)
    {
        buf[0] = address & 0xffff;
        buf[1] = address >> 16;
        return buf;
    }

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

#endif // _UTIL_
