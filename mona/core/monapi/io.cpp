/*! \file
    \brief monapi io functions

    \author Higepon
    \date   create:2004/10/30 update:$Date$
    $Revision$
*/
#include <monapi/io.h>
#include <monapi/syscall.h>
#include <monapi/Message.h>

/*!
    control irq interrupt.

    \param irq      [in] irq number 0-15
    \param enabled  [in] enable interrupt?
    \param auto_ir2 [in] if 8 <= irq <= 15 and enables = true, auto_ir2 indicate whether enable ir2.
    \return none
*/
void monapi_set_irq(int irq, MONAPI_BOOL enabled, MONAPI_BOOL auto_ir2)
{
    if (irq >= 0 && irq <= 7)
    {
        if (enabled == MONAPI_TRUE) outp8(0x21, (inp8(0x21) & ~(1 << irq)));
        else outp8(0x21, (inp8(0x21) | (1 << irq)));
    }
    else if (irq >= 8 && irq <= 15)
    {
        if (enabled == MONAPI_TRUE) outp8(0xa1, inp8(0xa1) & ~(1 << (irq - 8)));
        else outp8(0xa1, inp8(0xa1) | (1 << (irq - 8)));

        if (enabled == MONAPI_TRUE && auto_ir2 == MONAPI_TRUE)
        {
            outp8(0x21, (inp8(0x21) & ~(1 << 2)));
        }
    }
}

/*!
    wait interrupt.

    \param ms     [in] timeout ms
    \param irq    [in] irq number
    \param file   [in] file name for time out info.
    \param line   [in] line number for time out info.
    \return MONAPI_TRUE/MONAPI_FALSE OK/NG
*/
MONAPI_BOOL monapi_wait_interrupt(uint32_t ms, uint8_t irq, const char* file, int line)
{
    MessageInfo msg;

    uint32_t timerId = set_timer(ms);

    for (int i = 0; ; i++)
    {
        int result = MonAPI::Message::peek(&msg, i);

        if (result != 0)
        {
            i--;
            syscall_mthread_yield_message();
        }
        else if (msg.header == MSG_TIMER)
        {
            if (msg.arg1 != timerId) continue;
            kill_timer(timerId);

            MonAPI::Message::peek(&msg, i, PEEK_REMOVE);

            printf("interrupt timeout %s:%d\n", file, line);
            return MONAPI_FALSE;
        }
        else if (msg.header == MSG_INTERRUPTED)
        {
            if (msg.arg1 != irq) continue;
            kill_timer(timerId);

            MonAPI::Message::peek(&msg, i, PEEK_REMOVE);
            return MONAPI_TRUE;
        }
    }
    return MONAPI_FALSE;
}


uint8_t inp8(uint32_t port) {

    uint8_t ret;
    asm volatile ("inb %%dx, %%al": "=a"(ret): "d"(port));
    return ret;
}

void outp8(uint32_t port, uint8_t value) {
   asm volatile ("outb %%al, %%dx": :"d" (port), "a" (value));
}

uint16_t inp16(uint32_t port) {

    uint16_t ret;
    asm volatile ("inw %%dx, %%ax": "=a"(ret): "d"(port));
    return ret;
}

void outp16(uint32_t port, uint16_t value) {
   asm volatile ("outw %%ax, %%dx": :"d" (port), "a" (value));
}

uint32_t inp32(uint32_t port) {

    uint32_t ret;
    asm volatile ("inl %%dx, %%eax": "=a"(ret): "d"(port));
    return ret;
}

void outp32(uint32_t port, uint32_t value) {
   asm volatile ("outl %%eax, %%dx": :"d" (port), "a" (value));
}

