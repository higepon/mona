/*
 * Copyright (c) 2006 Christiaan Simons.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of and a contribution to the lwIP TCP/IP stack.
 *
 * Author: Christiaan Simons <christiaan.simons@axon.tv>
 */

/**
 * @file
 * Simple pre-allocated interval (reload) timers.
 * @see timer.h
 */

#include <monapi.h>
#include "timer.h"
#include "lwip/snmp.h"

//static struct itimerval tmr;

struct itmr
{
    volatile unsigned int interval;
    volatile unsigned int cnt;
    volatile unsigned char event;
};

static struct itmr timers[TIMER_NUM];

static void __fastcall timerThread(void* arg)
{
//    _printf("timer started");
    int intervalMsec = (int)arg;
    while(true) {
        _logprintf("net: %s:%d\n", __FILE__, __LINE__);
        sleep(intervalMsec);
        _logprintf("net: %s:%d\n", __FILE__, __LINE__);

//    snmp_inc_sysuptime();

        struct itmr* tp = &timers[TIMER_NUM - 1];
        for(unsigned char i = TIMER_NUM; i > 0; i--) {
            if (tp->interval != 0) {
                /* timer is running */
                if (tp->cnt == 0) {
                    /* timer expired */
                    tp->event |= 1;
                } else {
                    /* timer ticking */
                    tp->cnt--;
                }
            }
            tp--;
        }
    }
}


/**
 * Initializes interval timers.
 */
void timer_init(void)
{
    struct itmr *tp = &timers[TIMER_NUM-1];
    for(unsigned char i = TIMER_NUM; i > 0; i--) {
        tp->event = 0;
        tp->interval = 0;
        tp->cnt = 0;
        tp--;
    }

    const int INTERVAL_MSEC = 10;
        _logprintf("net: %s:%d\n", __FILE__, __LINE__);
    syscall_mthread_create_with_arg(timerThread, (void*)INTERVAL_MSEC);
        _logprintf("net: %s:%d\n", __FILE__, __LINE__);
}


/**
 * Configures timer.
 *
 * @param tmr the timer number from timer.h
 * @param interval when > 0 enables this timer, 0 disables.
 */
void timer_set_interval(unsigned char tmr, unsigned int interval)
{
    ASSERT(tmr < TIMER_NUM);
    timers[tmr].interval = interval;
}


/**
 * Returns timer event and restarts timer.
 */
unsigned char timer_testclr_evt(unsigned char tmr)
{
    ASSERT(tmr < TIMER_NUM);
    unsigned char evt;
    struct itmr *tp;

    tp = &timers[tmr];

    evt = tp->event;
    if (tp->event != 0) {
        tp->event = 0;
        tp->cnt = tp->interval;
    }
    return evt;
}


/**
 * interrupting (!) sigalarm handler
 */
void sigalarm_handler(int sig)
{
    unsigned char i;
    struct itmr *tp;

    snmp_inc_sysuptime();

    tp = &timers[TIMER_NUM-1];
    for(i = TIMER_NUM; i > 0; i--)
    {
        if (tp->interval != 0)
        {
            /* timer is running */
            if (tp->cnt == 0)
            {
                /* timer expired */
                tp->event |= 1;
            }
            else
            {
                /* timer ticking */
                tp->cnt--;
            }
        }
        tp--;
    }
}
