/*!
    \file  MFDCDriver.cpp
    \brief class Floppy Disk Controller for MultiTask

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/02/07 update:$Date$
*/

#include<MFDCDriver.h>
#include<monaIo.h>
#include<monaVga.h>

/* definition DOR */
#define FDC_MOTA_START  0x10
#define FDC_DMA_ENABLE  0x08
#define FDC_REST_RESET  0x00
#define FDC_REST_ENABLE 0x04
#define FDC_DR_SELECT_A 0x00

/* definition MSR */
#define FDC_MRQ_READY    0x80
#define FDC_DIO_TO_CPU   0x40
#define FDC_NDMA_NOT_DMA 0x20
#define FDC_BUSY_ACTIVE  0x10
#define FDC_ACTD_ACTIVE  0x08
#define FDC_ACTC_ACTIVE  0x04
#define FDC_ACTB_ACTIVE  0x02
#define FDC_ACTA_ACTIVE  0x01

/* definition on/off */
#define ON  true
#define OFF false

/* port address */
#define FDC_DOR_PRIMARY   0x3f2
#define FDC_DOR_SECONDARY 0x372
#define FDC_MSR_PRIMARY   0x3f4
#define FDC_MSR_SECONDARY 0x374
#define FDC_DR_PRIMARY    0x3f5
#define FDC_DR_SECONDARY  0x375


/* summary */
#define FDC_DOR_RESET   0
#define FDC_START_MOTOR (FDC_DMA_ENABLE | FDC_MOTA_START | FDC_REST_ENABLE | FDC_DR_SELECT_A)
#define FDC_STOP_MOTOR  (FDC_DMA_ENABLE | FDC_REST_ENABLE | FDC_DR_SELECT_A)

/* time out */
#define FDC_RETRY_MAX 100000

MFDCDriver* gMFDCDriver = 0;
bool MFDCDriver::interrupt_ = false;

/*!
    \brief Constructer

    \author HigePon
    \date   create:2003/02/03 update:
*/
MFDCDriver::MFDCDriver() {

    initilize();
    while (true);
}

/*!
    \brief Destructer

    \author HigePon
    \date   create:2003/02/03 update:
*/
MFDCDriver::~MFDCDriver() {

    motor(OFF);
    return;
}

/*!
    \brief initilize controller

    \author HigePon
    \date   create:2003/02/03 update:
*/
void MFDCDriver::initilize() {

    printStatus("before");

    /* reset drive */
    outportb(FDC_DOR_PRIMARY, FDC_DOR_RESET);

    printStatus("reset");

    /* start driveA */
    interrupt_ = false;
    motor(ON);
    while (!waitInterrupt());

    calibrate();

    printStatus("after");
    return;
}

/*!
    \brief print status of FDC

    \param  str message
    \author HigePon
    \date   create:2003/02/10 update:
*/
void MFDCDriver::printStatus(const char* str) const {

    byte msr = inportb(FDC_MSR_PRIMARY);
    _sys_printf("data reg |data flow|DMA|BUSY| D | C | B | A |int|\n");
    _sys_printf("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n"
              , msr & FDC_MRQ_READY    ? "  ready  ":"not ready"
              , msr & FDC_DIO_TO_CPU   ? " to CPU  ":" to Cont "
              , msr & FDC_NDMA_NOT_DMA ? " Y "      :" N "
              , msr & FDC_BUSY_ACTIVE  ? "  Y "     :"  N "
              , msr & FDC_ACTD_ACTIVE  ? " Y "      :" N "
              , msr & FDC_ACTC_ACTIVE  ? " Y "      :" N "
              , msr & FDC_ACTB_ACTIVE  ? " Y "      :" N "
              , msr & FDC_ACTA_ACTIVE  ? " Y "      :" N "
              , interrupt_ ? " T " : " F "
              , str
    );
}

void MFDCDriver::setFDCVersion() {


}

/*!
    \brief interrupt handler

    \author HigePon
    \date   create:2003/02/10 update:
*/
void MFDCDriver::interrupt() {

    interrupt_ = true;
}

/*!
    \brief wait interrupt

    \return true:interrupt
    \author HigePon
    \date   create:2003/02/10 update:
*/
bool MFDCDriver::waitInterrupt() {

    return interrupt_;
}

/*!
    \brief print status of FDC

    \param  on ON/OFF
    \author HigePon
    \date   create:2003/02/10 update:
*/
void MFDCDriver::motor(bool on) {

    if (on) outportb(FDC_DOR_PRIMARY, FDC_START_MOTOR);
    else outportb(FDC_DOR_PRIMARY, FDC_STOP_MOTOR);
    return;
}

bool MFDCDriver::sendCommand(const byte command[], const byte length) {

    /* check fdc status ready */
    if (!checkMSR(FDC_MRQ_READY)) {

        _sys_printf("MFDCDriver#sendCommand: timeout\n");
        return false;
    }

    /* send command */
    for (int i = 0; i < length; i++) {

        if (!checkMSR(FDC_MRQ_READY | FDC_DIO_TO_CPU)) {
	    _sys_printf("pattern %x", FDC_MRQ_READY | FDC_DIO_TO_CPU);
            _sys_printf("MFDCDriver#sendCommand: timeout[%d]\n", i);
            return false;
        }
        outportb(FDC_DR_PRIMARY, command[i]);
    }

    return true;
}

/*!
    \brief calibrate

    \return true OK/false command fail
    \author HigePon
    \date   create:2003/02/10 update:
*/
bool MFDCDriver::calibrate() {

    byte command[] = {0x07, 0x00}; /* calibrate */

    interrupt_ = false;
    if(!sendCommand(command, sizeof(command))){

        _sys_printf("MFDCDriver#calibrate:command fail\n");
        return false;
    }

    while(!waitInterrupt());
}

/*!
    \brief wait until FDC is ready.

    \param  expectedcondition
    \return true ready/false time out
    \author HigePon
    \date   create:2003/02/10 update:
*/
bool MFDCDriver::checkMSR(byte expectedCondition) {

    /* check whether FDC is ready or not */
    for (dword i = 0; i < FDC_RETRY_MAX; i++) {

        bool isMSRReady = (inportb(FDC_MSR_PRIMARY) & expectedCondition) == expectedCondition;
        if (isMSRReady) return true;
    }

    /* time out */
    return false;
}
