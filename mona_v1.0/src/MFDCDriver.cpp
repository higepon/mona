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

/* port address */
#define FDC_DOR_PRIMARY   0x3f2
#define FDC_DOR_SECONDARY 0x372
#define FDC_MSR_PRIMARY   0x3f4
#define FDC_MSR_SECONDARY 0x374

/* summary */
#define FDC_DOR_RESET   0
#define FDC_START_DRIVE (FDC_DMA_ENABLE | FDC_MOTA_START | FDC_REST_ENABLE | FDC_DR_SELECT_A)

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
    _sys_printf("FDC_DOR_RESET=%x\n", FDC_DOR_RESET);
    outportb(FDC_DOR_PRIMARY, FDC_DOR_RESET);

    printStatus("reset");

    /* start driveA */
    interrupt_ = false;
    outportb(FDC_DOR_PRIMARY, FDC_START_DRIVE);
    while (!waitInterrupt());

    printStatus("after");
    return;
}

void MFDCDriver::printStatus(const char* str) const {

    byte mrq = inportb(FDC_MSR_PRIMARY);
    _sys_printf("data reg |data flow|DMA|BUSY| D | C | B | A |int|\n");
    _sys_printf("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n"
              , mrq & FDC_MRQ_READY    ? "  ready  ":"not ready"
              , mrq & FDC_DIO_TO_CPU   ? " to CPU  ":" to Cont "
              , mrq & FDC_NDMA_NOT_DMA ? " Y "      :" N "
              , mrq & FDC_BUSY_ACTIVE  ? "  Y "     :"  N "
              , mrq & FDC_ACTD_ACTIVE  ? " Y "      :" N "
              , mrq & FDC_ACTC_ACTIVE  ? " Y "      :" N "
              , mrq & FDC_ACTB_ACTIVE  ? " Y "      :" N "
              , mrq & FDC_ACTA_ACTIVE  ? " Y "      :" N "
              , interrupt_ ? " T " : " F "
              , str
    );
}

void MFDCDriver::setFDCVersion() {


}

void MFDCDriver::interrupt() {

    interrupt_ = true;
}

bool MFDCDriver::waitInterrupt() {

    return interrupt_;
}

