#include<tester.h>
#include<FDCDriver.h>
#include<global.h>
#include<io.h>
#include<GraphicalConsole.h>
#include<operator.h>
#include<z.h>
#include<MemoryManager.h>
#include<KeyBoardManager.h>
#include<elf.h>
#include<syscalls.h>
#include<Process.h>
#include<vbe.h>
#include<pic.h>

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

    if (wakeupResult != 0)
    {
        ThreadOperation::switchThread((wakeupResult == 1), 78);
    }

    /* not reached */
}

const kevent KEvent::MESSAGE_COME  = 0x0001;
const kevent KEvent::FDC_INTERRUPT = 0x0002;

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
int Mouse::init() {

    /*
       status = inp8(0x64);

       bit0: out buffer full 1
       bit1: in  buffer full 1

       KBC writable<outp8(0x60,)> when bit0 and bit1 = 0
       KBC readable<inp8(0x60)>   when bit0 = 1

    */

// bochs hate this interface test */

    /* aux interface test */
//     outp8(0x64, 0xa9);
//     if (waitReadable()) {
//         return 1;
//     }

//     if (inp8(0x60)) {

//         /* aux interface test error */
//         return 2;
//     }

// bochs comment out

    /* enable aux */
    outp8(0x64, 0xa8);

    /* get command written before */
    byte data;
    outp8(0x64, 0x20);
    if (waitReadable()) {
        return 3;
    }
    data = inp8(0x60);

    /* kbc command write keyboard & enable mouse intterupt */
    outp8(0x64, 0x60);
    if (waitWritable()) {
        return 4;
    }
    outp8(0x60, data | 0x03);

    /* after kbc command write, read one data */
//     if (waitReadable()) {
//         return 5;
//     }
//     data = inp8(0x60);
//     not necesarry? above?

    /* mouse reset */
    outp8(0x64, 0xd4);
    if (waitWritable()) {
        return 6;
    }
    outp8(0x60, 0xff);

    /* after kbc command write, read 3 times */
    if (waitReadable()) {
        return 7;
    }
    data = inp8(0x60);

    if (waitReadable()) {
        return 8;
    }
    data = inp8(0x60);

    if (waitReadable()) {
        return 9;
    }
    data = inp8(0x60);

    /* enable mouse */
    outp8(0x64, 0xd4);
    if (waitWritable()) {
        return 10;
    }
    outp8(0x60, 0xf4);

    /* after enable mouse read one data */
    if (waitReadable()) {
        return 11;
    }
    data = inp8(0x60);

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

    info = allocateMessageInfo();

    *info = *message;
    info->from = g_currentThread->thread->id;
    thread->messageList->add(info);

    KEvent::set(thread, KEvent::MESSAGE_COME);
    return 0;
}

int Messenger::receive(Thread* thread, MessageInfo* message)
{
    MessageInfo* from = thread->messageList->get(0);

    if (from == (MessageInfo*)NULL)
    {
        return -1;
    }

    *message = *from;
    thread->messageList->removeAt(0);
    return 0;
}

