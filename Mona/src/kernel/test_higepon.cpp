#include "tester.h"
#include "FDCDriver.h"
#include "global.h"
#include "io.h"
#include "GraphicalConsole.h"
#include "operator.h"
#include "MemoryManager.h"
#include "syscalls.h"
#include "Process.h"
#include "vbe.h"
#include "pic.h"

/*!

    \brief  test code for higepon

    Copyright (c) 2002,2003 Higepon and the individuals listed on the ChangeLog entries.
    All rights reserved.
    License=MIT/X Licnese

    \author  HigePon
    \version $Revision$
    \date   create:2003/05/18 update:$Date$
*/

/*----------------------------------------------------------------------
    RTC
----------------------------------------------------------------------*/
void RTC::init() {

    /* 24h */
    write(0x0B, 0x02);
}

void RTC::write(byte reg, byte value) {

    dword eflags = get_eflags();
    disableInterrupt();

    outp8(RTC_ADRS, reg);
    outp8(RTC_DATA, value);
    set_eflags(eflags);
}

byte RTC::read(byte reg) {

    byte result;
    dword eflags = get_eflags();
    disableInterrupt();

    outp8(RTC_ADRS, (byte)(reg & 0xff));
    result = inp8(RTC_DATA);

    set_eflags(eflags);
    return result;
}

int RTC::readDateOnce(KDate* date) {

    date->year      = convert(read(RTC_YEAR)) + 2000;
    date->month     = convert(read(RTC_MONTH));
    date->day       = convert(read(RTC_DAY));
    date->dayofweek = convert(read(RTC_DOW));
    date->hour      = convert(read(RTC_HOUR));
    date->min       = convert(read(RTC_MIN));
    date->sec       = convert(read(RTC_SEC));
    return date->min;
}

void RTC::getDate(KDate* date) {

    for (;;) {
        int once  = readDateOnce(date);
        int twice = readDateOnce(date);
        if (once == twice) break;
    }
    return;
}

void rdtsc(dword* timeL, dword* timeH) {

    dword l,h;
    asm volatile("rdtsc           \n"
                 "mov   %%eax, %0 \n"
                 "mov   %%edx, %1 \n"
                 : "=m"(l), "=m"(h)
                 : /* no */
                 : "eax", "edx");
    *timeL = l;
    *timeH = h;
}

void rdtscsub(dword* timeL, dword* timeH) {

    dword l = *timeL;
    dword h = *timeH;

    asm volatile("rdtsc           \n"
                 "sub   %2, %%eax \n"
                 "sub   %3, %%edx \n"
                 "mov   %%eax, %0 \n"
                 "mov   %%edx, %1 \n"
                 : "=m"(l), "=m"(h)
                 : "m"(l), "m"(h)
                 : "eax", "edx");

    *timeL = l;
    *timeH = h;
}

/*----------------------------------------------------------------------
    Messenger
----------------------------------------------------------------------*/
Messenger::Messenger(int size) : size_(size), allocated_(0) {

    info_ = new MessageInfo[size];
}

Messenger::~Messenger() {
}

MessageInfo* Messenger::allocateMessageInfo() {

    MessageInfo* result = &(info_[allocated_]);
    allocated_++;
    if (allocated_ > size_ - 1) {

#if 0  // DEBUG for message
        g_console->printf("***** msg buf index set to zero again ****");
#endif
        allocated_ = 0;
    }
    return result;
}

int Messenger::send(dword id, MessageInfo* message)
{
    Thread* thread;
    MessageInfo* info;

    if (message == (MessageInfo*)NULL)
    {
        return -1;
    }

    if ((thread = g_scheduler->Find(id)) == (Thread*)NULL)
    {
        return -1;
    }

    info = allocateMessageInfo();

#if 0
    logprintf("send:to=%x head=%x a1=%x a2=%x a3=%x from=%x\n"
              , id
              , message->header
              , message->arg1
              , message->arg2
              , message->arg3
              , message->from
        );
#endif

    *info = *message;
    info->from = g_currentThread->thread->id;

    thread->flags |= MEvent::MESSAGE;
    thread->messageList->add(info);

    g_scheduler->EventComes(thread, MEvent::MESSAGE);

    return 0;
}

int Messenger::receive(Thread* thread, MessageInfo* message)
{
    MessageInfo* from = thread->messageList->removeAt(0);

    if (from == (MessageInfo*)NULL)
    {
        return -1;
    }

#if 0
    logprintf("recv:to=%x head=%x a1=%x a2=%x a3=%x from=%x\n"
              , thread->id
              , message->header
              , message->arg1
              , message->arg2
              , message->arg3
              , message->from
        );
#endif

    thread->flags &= ~MEvent::MESSAGE;
    *message = *from;

    return 0;
}

int Messenger::peek(Thread* thread, MessageInfo* message, int index, int flags)
{
    List<MessageInfo*>* list = thread->messageList;

    if (index > list->size())
    {
        return 1;
    }

    MessageInfo* from = flags & PEEK_REMOVE ? list->removeAt(index) : list->get(index);

    if (from == (MessageInfo*)NULL)
    {
        return -1;
    }

    thread->flags &= ~MEvent::MESSAGE;
    *message = *from;
    return 0;
}
