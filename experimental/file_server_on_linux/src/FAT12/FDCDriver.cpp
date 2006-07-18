/*!
    \file  FDCDriver.cpp
    \brief class Floppy Disk Controller for MultiTask
    \brief remove from kernel and add to file server

    Copyright (c) 2002,2003 Higepon
    All rights reserved.
    License=MIT/X License

    \author  HigePon
    \version $Revision: 2701 $
    \date   create:2003/02/07 update:$Date: 2005-07-17 22:28:15 +0900 (日, 17  7月 2005) $
*/

#include "FDCDriver.h"

#ifdef ON_LINUX
#else
using namespace MonAPI;
#endif

/*!
    \brief Constructer

    \author HigePon
    \date   create:2003/02/03 update:2004/12/27
*/
FDCDriver::FDCDriver(FILE* fp) : motorCount(0), currentTrack(-1), fp(fp)
{
    return;
}

/*!
    \brief Destructer

    \author HigePon
    \date   create:2003/02/03 update:2004/12/27
*/
FDCDriver::~FDCDriver()
{
    return;
}

/*!
    \brief initilize controller

    \author HigePon
    \date   create:2003/02/03 update:2004/12/27
*/
void FDCDriver::initilize()
{
    return;
}

int FDCDriver::open()
{
    return 0;
}

int FDCDriver::close()
{
    return 0;
}

int FDCDriver::read(dword lba, void* buf, int size)
{
    fseek(this->fp, lba * 512, SEEK_SET);
    int result = fread(buf, size, 1, fp);
    return result;
}

int FDCDriver::write(dword lba, void* buf, int size)
{
    fseek(fp, lba * 512, SEEK_SET);
    int result = fwrite(buf, size, 1, fp);
    return result;
}

/*!
    \brief wait interrupt

    \author HigePon
    \date   create:2003/02/10 update:2004/12/27
*/
void FDCDriver::waitInterrupt()
{
}

/*!
    \brief print status of FDC

    \param  on ON/OFF
    \author HigePon
    \date   create:2003/02/10 update:2004/12/27
*/
void FDCDriver::motor(bool on)
{
    return;
}

/*!
    \brief print status of FDC

    \author HigePon
    \date   create:2004/02/10 update:2004/12/27
*/
void FDCDriver::motorAutoOff()
{
}

/*!
    \brief send command to FDC

    \param  command array of command
    \param  length  length of command
    \author HigePon
    \date   create:2003/02/16 update:2004/12/27
*/
bool FDCDriver::sendCommand(const byte* command, const byte length)
{
    return true;
}

/*!
    \brief recalibrate

    \return true OK/false command fail
    \author HigePon
    \date   create:2003/02/10 update:2004/12/27
*/
bool FDCDriver::recalibrate()
{
    return true;
}

/*!
    \brief wait for FDC status

    \param  expected wait until msr == expected
    \author HigePon
    \date   create:2003/09/19 update:2004/12/27
*/
void FDCDriver::waitStatus(byte expected)
{
}

/*!
    \brief wait for FDC status

    \param  expected wait until (msr & mask) == expected
    \param  mask     wait until (msr & mask) == expected
    \author HigePon
    \date   create:2003/09/19 update:2004/12/27
*/
void FDCDriver::waitStatus(byte mask, byte expected)
{
}

/*!
    \brief get result of result phase

    \return result
    \author HigePon
    \date   create:2003/09/19 update:2004/12/27
*/
byte FDCDriver::getResult()
{
    return 0;
}

/*!
    \brief seek

    \param  track
    \return true OK/false time out
    \author HigePon
    \date   create:2003/02/11 update:2004/12/27
*/
bool FDCDriver::seek(byte track)
{
    return true;
}

/*!
    \brief Sense Interrrupt Command

    \author HigePon
    \date   create:2003/02/13 update:2004/12/27
*/
bool FDCDriver::senseInterrupt()
{
    return true;
}

/*!
    \brief start dma

    \author HigePon
    \date   create:2003/02/15 update:2004/12/27
*/
void FDCDriver::startDMA()
{
    return;
}

/*!
    \brief stop dma

    \author HigePon
    \date   create:2003/02/15 update:2004/12/27
*/
void FDCDriver::stopDMA()
{
    return;
}

/*!
    \brief setup dmac for read

    \author HigePon
    \date   create:2003/02/15 update:2004/12/27
*/
void FDCDriver::setupDMARead(dword size)
{
    return;
}

/*!
    \brief setup dmac for write

    \author HigePon
    \date   create:2003/02/15 update:2004/12/27
*/
void FDCDriver::setupDMAWrite(dword size)
{
    return;
}

/*!
    \brief disk read

    \param track  track
    \param head   head
    \param sector sector

    \author HigePon
    \date   create:2003/02/15 update:2004/12/27
*/
bool FDCDriver::read(byte track, byte head, byte sector)
{
    return true;
}

/*!
    \brief disk write

    \param track  track
    \param head   head
    \param sector sector

    \author HigePon
    \date   create:2003/02/15 update:2004/12/27
*/
bool FDCDriver::write(byte track, byte head, byte sector)
{
    return true;
}

/*!
    \brief disk read

    \param lba    logical block address
    \param buf    read result buffer 512byte

    \author HigePon
    \date   create:2003/02/15 update:2004/12/27
*/
bool FDCDriver::read(dword lba, byte* buf)
{
    return false;
}

/*!
    \brief disk write

    \param lba    logical block address
    \param buf    write result buffer 512byte

    \author HigePon
    \date   create:2003/02/15 update:2004/12/27
*/
bool FDCDriver::write(dword lba, byte* buf)
{
    return false;
}

/*!
    \brief disk read

    \param lba    logical block address
    \param track  track
    \param head   head
    \param sector sector

    \author HigePon
    \date   create:2003/02/15 update:2004/12/27
*/
void FDCDriver::lbaToTHS(int lba, byte& track, byte& head, byte& sector)
{
    return;
}

int FDCDriver::ioctl(void* p)
{
    return 0;
}

bool FDCDriver::checkDiskChange()
{
    return true;
}
