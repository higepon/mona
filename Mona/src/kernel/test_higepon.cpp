#include "tester.h"
#include "FDCDriver.h"
#include "global.h"
#include "io.h"
#include "GraphicalConsole.h"
#include "operator.h"
#include "MemoryManager.h"
#include "KeyBoardManager.h"
#include "elf.h"
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
    KEvent
----------------------------------------------------------------------*/
/* this function should be used on system call or interrupt handler */
void KEvent::wait(Thread* thread, kevent e)
{
    g_scheduler->wait(thread, e);
    bool isProcessChange = g_scheduler->schedule();
    ThreadOperation::switchThread(isProcessChange, 79);

    /* not reached */
}

void KEvent::set(Thread* thread, kevent e)
{
    int wakeupResult = g_scheduler->wakeup(thread, e);

    if (e == MUTEX_LOCKED) g_console->printf("result=%x", wakeupResult);

    if (wakeupResult != 0)
    {
        ThreadOperation::switchThread((wakeupResult == 1), 78);
    }

    /* not reached */
}

const kevent KEvent::MESSAGE_COME  = 0x0001;
const kevent KEvent::FDC_INTERRUPT = 0x0002;
const kevent KEvent::MUTEX_LOCKED  = 0x0003;

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
    Mouse
----------------------------------------------------------------------*/
int Mouse::init()
{
    byte data;
    outp8(0x64, 0x20);
    if (waitReadable())
    {
        return 1;
    }
    data = inp8(0x60);

    outp8(0x64, 0x60);
    if (waitWritable())
    {
        return 2;
    }

    outp8(0x60, data & (~0x30) | 0x3);
    if (!waitReadable())
    {
        inp8(0x60);
    }

    outp8(0x64, 0xd4);
    if (waitWritable())
    {
        return 4;
    }
    outp8(0x60, 0xf4);
    if (waitReadable())
    {
        return 5;
    }
    inp8(0x60);
    return 0;
}

int Mouse::waitWritable() {

    byte status;
    int i;

    for (i = 0, status = inp8(0x64); i < MOUSE_TIMEOUT; i++, status = inp8(0x64)) {

        /* writable */
        if ((status & 0x03) == 0x00) {
            break;
        }
    }
    return (i == MOUSE_TIMEOUT) ? -1 : 0;
}

int Mouse::waitReadable() {

    byte status;
    int i;

    for (i = 0, status = inp8(0x64); i < MOUSE_TIMEOUT; i++, status = inp8(0x64)) {

        /* readable */
        if ((status & 0x01) == 0x01) {
            break;
        }
    }

    return (i == MOUSE_TIMEOUT) ? -1 : 0;
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

    if ((thread = g_scheduler->find(id)) == (Thread*)NULL)
    {
        return -1;
    }

    dword eflags = get_eflags();
    disableInterrupt();
    info = allocateMessageInfo();
    set_eflags(eflags);

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

#if 0  // DEBUG for message
    if (id == 60 || id == 63) g_console->printf("@%d->%d@", info->from, id);
#endif

    enter_kernel_lock_mode();
    thread->messageList->add(info);
    exit_kernel_lock_mode();

    KEvent::set(thread, KEvent::MESSAGE_COME);

    return 0;
}

int Messenger::receive(Thread* thread, MessageInfo* message)
{
    enter_kernel_lock_mode();
    MessageInfo* from = thread->messageList->removeAt(0);
    exit_kernel_lock_mode();

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

    *message = *from;

    return 0;
}
