/*!
    \file  IDEDriver.cpp
    \brief IDEDriver

    Copyright (c) 2004 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision: 3123 $
    \date   create:2004/11/14 update:$Date: 2006-04-15 13:55:21 +0900 (土, 15  4月 2006) $
*/

#include "IDEDriver.h"
#ifdef ON_LINUX
#include <string.h>
#define sleep //
#else
#include <monapi.h>
#include <Assert.h>
#include <monapi/io.h>
#endif


using namespace MonAPI;

//#define DEBUG_READ_TRACE

/*----------------------------------------------------------------------
    IDEDRIVER
----------------------------------------------------------------------*/
IDEDriver::IDEDriver(int irq_primary, int irq_secondary)
{
}

IDEDriver::~IDEDriver()
{
}

/*----------------------------------------------------------------------
    IDEDRIVER : for user operation
----------------------------------------------------------------------*/
bool IDEDriver::selectDevice(int controller, int deviceNo)
{
    return true;
}

bool IDEDriver::findDevice(int type, int detail, int* controller, int* deviceNo)
{
    return false;
}

int IDEDriver::read(dword lba, void* buffer, int size)
{
    fseek(this->fp, lba * 512, SEEK_SET);
    int result = fread(buffer, size, 1, this->fp);
    return result;
}

int IDEDriver::open()
{
    this->fp = fopen("mona.iso", "rb+");
    if (this->fp == NULL)
    {
        fprintf(stderr, "mona.iso not found");
        exit(1);
    }
}

int IDEDriver::close()
{
    fclose(this->fp);
    return 0;
}

int IDEDriver::write(dword lba, void* buf, int size)
{
    fseek(this->fp, lba * 512, SEEK_SET);
    int result = fread(buf, size, 1, fp);
    return result;
}

int IDEDriver::ioctl(void* p)
{
    return 0;
}

int IDEDriver::getLogicalBlockSize()
{
    return 0;
}

/*----------------------------------------------------------------------
    IDEDRIVER : error information
----------------------------------------------------------------------*/
int IDEDriver::getLastError() const
{
    return this->lastError;
}

void IDEDriver::getLastErrorDetail(byte* buffer)
{
}

/*----------------------------------------------------------------------
    IDEDRIVER : I/O
----------------------------------------------------------------------*/
void IDEDriver::outp8(IDEController* controller, int reg, byte value)
{

}

byte IDEDriver::inp8(IDEController* controller, int reg)
{
    return 0;
}


word IDEDriver::inp16(IDEController* controller, int reg)
{
    return 0;
}

void IDEDriver::outp16(IDEController* controller, int reg, word value)
{
}

void IDEDriver::outp16(IDEController* controller, word* data, int length)
{
}

void IDEDriver::inp16(IDEController* controller, word* data, int size)
{
}

/*----------------------------------------------------------------------
    IDEDRIVER : flag utilities
----------------------------------------------------------------------*/
bool IDEDriver::waitBusyAndDataRequestBothClear(IDEController* controller)
{
    return true;
}

bool IDEDriver::waitBusyClear(IDEController* controller)
{
    return true;
}

bool IDEDriver::waitDrdySet(IDEController* controller)
{
    return true;
}

/*----------------------------------------------------------------------
    IDEDRIVER : protocol
----------------------------------------------------------------------*/
bool IDEDriver::protocolPacket(IDEController* controller, ATAPICommand* command)
{
    return true;
}

bool IDEDriver::protocolAtaNoneData(IDEController* controller, ATACommand* command)
{

    return true;
}

bool IDEDriver::protocolPioDataIn(IDEController* controller, ATACommand* command, word count, void* buf)
{
    return true;
}

void IDEDriver::protocolInterrupt()
{
}

/*----------------------------------------------------------------------
    IDEDRIVER : execute command using protocol function
----------------------------------------------------------------------*/
bool IDEDriver::commandIdleImmediate(IDEController* controller, int deviceNo)
{
    return true;
}

bool IDEDriver::commandRequestSense(IDEController* controller)
{
    return true;
}

bool IDEDriver::commandRead10(IDEController* controller, dword lba, void* buffer, int size)
{
    return true;
}

bool IDEDriver::commandIdentify(IDEController* controller, int deviceNo, word* buffer)
{
    return true;
}

/*----------------------------------------------------------------------
    IDEDRIVER :initialize functions
----------------------------------------------------------------------*/
void IDEDriver::initialize(IDEController* controller)
{
}

/*
    call only after software reset
*/
void IDEDriver::setDeviceTypeFirst(IDEController* controller, int deviceNo)
{
}

void IDEDriver::setDeviceTypeSecond(IDEController* controller, int deviceNo)
{
}

bool IDEDriver::selectDevice(IDEController* controller, int deviceNo)
{
    return true;
}

byte IDEDriver::deviceValue(int deviceNo) const
{
    return true;
}
