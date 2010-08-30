/*
 *   Copyright (c) 2010  Higepon(Taro Minowa)  <higepon@users.sourceforge.jp>
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
 */
#ifndef _KSTATS_
#define _KSTATS_

#define MAX_KSTATS 10

enum
{
    PAGE_FAULT1,
    forbidden_comma
};

struct KStat
{
    KStat()
    {
        memset(stats, 0, MAX_KSTATS);
        memset(statsCount, 0, MAX_KSTATS);
        memset(statsTemp, 0, MAX_KSTATS);
    }
    uint64_t stats[MAX_KSTATS];
    uint64_t statsCount[MAX_KSTATS];
    uint64_t statsTemp[MAX_KSTATS];
    void startIncrementByTSC(int statIndex)
    {
        if (statIndex >= MAX_KSTATS) {
            return;
        }
        statsTemp[statIndex] = rdtsc();
        statsCount[statIndex]++;
    }

    void stopIncrementByTSC(int statIndex)
    {
        if (statIndex >= MAX_KSTATS) {
            return;
        }
        stats[statIndex] += rdtsc() - statsTemp[statIndex];
    }

    uint64_t rdtsc()
    {
        uint32_t l,h;
        asm volatile("rdtsc           \n"
                     "mov   %%eax, %0 \n"
                     "mov   %%edx, %1 \n"
                     : "=m"(l), "=m"(h)
                     : /* no */
                     : "eax", "edx");
        return ((uint64_t)h << 32) | l;
    }
};

#endif
