/*!
    \file  FDCDriver.cpp
    \brief class Floppy Disk Controller for MultiTask

    Copyright (c) 2002,2003 Higepon
    All rights reserved.
    License=MIT/X Licnese

    \author  HigePon
    \version $Revision$
    \date   create:2003/02/07 update:$Date$
*/

#include<FDCDriver.h>
#include<kernel.h>
#include<operator.h>
#include<io.h>
#include<global.h>
#include<string.h>
#include<syscalls.h>
#include<SystemInfo.h>

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
#define FDC_DR_PRIMARY    0x3f5
#define FDC_DR_SECONDARY  0x375
#define FDC_CCR_PRIMARY   0x3f7
#define FDC_CCR_SECONDARY 0x377

#define FDC_DMA_S_SMR     0x0a
#define FDC_DMA_S_MR      0x0b
#define FDC_DMA_S_CBP     0x0c
#define FDC_DMA_S_BASE    0x04
#define FDC_DMA_S_COUNT   0x05
#define FDC_DMA_PAGE2     0x81

/* summary */
#define FDC_DOR_RESET   0
#define FDC_START_MOTOR (FDC_DMA_ENABLE | FDC_MOTA_START | FDC_REST_ENABLE | FDC_DR_SELECT_A)
#define FDC_STOP_MOTOR  (FDC_DMA_ENABLE | FDC_REST_RESET | FDC_DR_SELECT_A)

/* FDC Commands */
#define FDC_COMMAND_SEEK            0x0f
#define FDC_COMMAND_SENSE_INTERRUPT 0x08
#define FDC_COMMAND_SPECIFY         0x03
#define FDC_COMMAND_READ            0xe6 // bochs & VPC
//#define FDC_COMMAND_READ            0x46

/* time out */
#define FDC_RETRY_MAX 600000

#define FDC_DMA_BUFF_SIZE 512

bool FDCDriver::interrupt_ ;

/*!
    \brief Constructer

    \author HigePon
    \date   create:2003/02/03 update:2003/09/20
*/
FDCDriver::FDCDriver() {

    initilize();
    return;
}

/*!
    \brief Destructer

    \author HigePon
    \date   create:2003/02/03 update:2003/09/23
*/
FDCDriver::~FDCDriver() {

    motor(OFF);
    free(dmabuff_);
    return;
}

/*!
    \brief initilize controller

    \author HigePon
    \date   create:2003/02/03 update:
*/
void FDCDriver::initilize() {

    byte specifyCommand[] = {FDC_COMMAND_SPECIFY
                           , 0xC1 /* SRT = 4ms HUT = 16ms */
                           , 0x10 /* HLT = 16ms DMA       */
                            };

    /* allocate dma buffer */
    dmabuff_ = (byte*)malloc(FDC_DMA_BUFF_SIZE);

    /* dma buff should be 64kb < dma buff < 16Mb */
    if (!dmabuff_ || (dword)dmabuff_ < 64 * 1024 || (dword)dmabuff_  + FDC_DMA_BUFF_SIZE > 16 * 1024 * 1024) {
        panic("dma buff allocate error");
    }

    g_console->printf("dma=%x\n", (dword)dmabuff_);

    /* setup DMAC */
    outportb(0xda, 0x00);
    delay(1);
    outportb(0x0d, 0x00);
    delay(1);
    outportb(0xd0, 0x00);
    delay(1);
    outportb(0x08, 0x00);
    delay(1);
    outportb(0xd6, 0xc0);
    delay(1);
    outportb(0x0b, 0x46);
    delay(1);
    outportb(0xd4, 0x00);
    delay(1);

    /* reset drive */
    outportb(FDC_DOR_PRIMARY, FDC_DOR_RESET);

    delay(1);
    outportb(FDC_CCR_PRIMARY, 0);

    motor(ON);

    /* specify */
    if (!sendCommand(specifyCommand, sizeof(specifyCommand))) {

        info(ERROR, "Specify command failed\n");
        motor(OFF);
        return;
    }

    motor(OFF);
    return;
}

/*!
    \brief interrupt handler

    \author HigePon
    \date   create:2003/02/10 update:
*/
void FDCDriver::interrupt() {

    //    info(DEV_WARNING, "\ninterrupt:"); // here is point
    //    g_console->printf("s_interrupt");
    interrupt_ = true;
    //    g_console->printf("e_interrupt");
}

/*!
    \brief wait interrupt

    \author HigePon
    \date   create:2003/02/10 update:2003/09/19
*/
void FDCDriver::waitInterrupt() {

    while (!interrupt_);
}

/*!
    \brief print status of FDC

    \param  on ON/OFF
    \author HigePon
    \date   create:2003/02/10 update:2003/05/18
*/
void FDCDriver::motor(bool on) {

    if (on) {
        interrupt_ = false;
        outportb(FDC_DOR_PRIMARY, FDC_START_MOTOR);

        delay(4);

    } else outportb(FDC_DOR_PRIMARY, FDC_STOP_MOTOR);
    return;
}

/*!
    \brief send command to FDC

    \param  command array of command
    \param  length  length of command
    \author HigePon
    \date   create:2003/02/16 update:2003/09/18
*/
bool FDCDriver::sendCommand(const byte* command, const byte length) {

    /* send command */
    for (int i = 0; i < length; i++) {

        waitStatus(0x80 | 0x40, 0x80);

        /* send command */
        outportb(FDC_DR_PRIMARY, command[i]);
    }

    return true;
}

/*!
    \brief recalibrate

    \return true OK/false command fail
    \author HigePon
    \date   create:2003/02/10 update:2003/09/18
*/
bool FDCDriver::recalibrate() {

    byte command[] = {0x07, 0x00}; /* recalibrate */

    interrupt_ = false;
    if (!sendCommand(command, sizeof(command))){

        info(ERROR, "FDCDriver#recalibrate:command fail\n");
        return false;
    }

    while (true) {

        waitInterrupt();

        waitStatus(0x10, 0x00);

        if (senseInterrupt()) break;
        interrupt_ = false;
    }
    return true;
}

/*!
    \brief wait for FDC status

    \param  expected wait until msr == expected
    \author HigePon
    \date   create:2003/09/19 update:
*/
void FDCDriver::waitStatus(byte expected) {

    byte status;

    do {

        status = inportb(FDC_MSR_PRIMARY);

    } while (status != expected);
}

/*!
    \brief wait for FDC status

    \param  expected wait until (msr & mask) == expected
    \param  mask     wait until (msr & mask) == expected
    \author HigePon
    \date   create:2003/09/19 update:
*/
void FDCDriver::waitStatus(byte mask, byte expected) {

    byte status;

    do {

        status = inportb(FDC_MSR_PRIMARY);

    } while ((status & mask) != expected);
}

/*!
    \brief get result of result phase

    \return result
    \author HigePon
    \date   create:2003/09/19 update:
*/
byte FDCDriver::getResult() {

    waitStatus(0xd0, 0xd0);
    return inportb(FDC_DR_PRIMARY);
}

/*!
    \brief seek

    \param  track
    \return true OK/false time out
    \author HigePon
    \date   create:2003/02/11 update:2003/09/19
*/
bool FDCDriver::seek(byte track) {

    byte command[] = {FDC_COMMAND_SEEK, 0, track};

    interrupt_ = false;
    if (!sendCommand(command, sizeof(command))){

        info(ERROR, "FDCDriver#:seek command fail\n");
        return false;
    }

    while (true) {

        //   info(DEV_WARNING, "seek start3 \n");// here is point

        waitInterrupt();

        waitStatus(0x10, 0x00);

        if (senseInterrupt()) break;
        interrupt_ = false;
    }

    //    info(DEV_WARNING, "seek start4 \n"); // here is point
    return true;
}

/*!
    \brief Sense Interrrupt Command

    \author HigePon
    \date   create:2003/02/13 update:2003/09/19
*/
bool FDCDriver::senseInterrupt() {

    byte command[] = {FDC_COMMAND_SENSE_INTERRUPT};

    if (!sendCommand(command, sizeof(command))){

        info(ERROR, "FDCDriver#senseInterrrupt:command fail\n");
        return false;
    }

    results_[0] = getResult(); /* ST0 */
    results_[1] = getResult(); /* PCN */

    if ((results_[0] & 0xC0) != 0x00) return false;

    return true;
}

/*!
    \brief start dma

    \author HigePon
    \date   create:2003/02/15 update:
*/
void FDCDriver::startDMA() {

    /* mask channel2 */
    outportb(FDC_DMA_S_SMR, 0x02);
    return;
}

/*!
    \brief stop dma

    \author HigePon
    \date   create:2003/02/15 update:
*/
void FDCDriver::stopDMA() {

    /* unmask channel2 */
    outportb(FDC_DMA_S_SMR, 0x06);
    return;
}

/*!
    \brief setup dmac for read

    \author HigePon
    \date   create:2003/02/15 update:2003/05/25
*/
void FDCDriver::setupDMARead(dword size) {

    size--; /* size should be */
    dword p = (dword)dmabuff_;

    stopDMA();

    /* direction write */
    outportb(FDC_DMA_S_MR, 0x46);

    enter_kernel_lock_mode();

    /* clear byte pointer */
    outportb(FDC_DMA_S_CBP, 0);
    outportb(FDC_DMA_S_BASE,  byte(p & 0xff));
    outportb(FDC_DMA_S_BASE,  byte((p >> 8) & 0xff));
    outportb(FDC_DMA_S_COUNT, byte(size & 0xff));
    outportb(FDC_DMA_S_COUNT, byte(size >>8));
    outportb(FDC_DMA_PAGE2  , byte((p >>16)&0xFF));

    exit_kernel_lock_mode();

    startDMA();
    return;
}

/*!
    \brief setup dmac for write

    \author HigePon
    \date   create:2003/02/15 update:
*/
void FDCDriver::setupDMAWrite(dword size) {

    size--;
    dword p = (dword)dmabuff_;

    stopDMA();

    /* direction read */
    outportb(FDC_DMA_S_MR, 0x4a);

    enter_kernel_lock_mode();

    /* clear byte pointer */
    outportb(FDC_DMA_S_CBP, 0);
    outportb(FDC_DMA_S_BASE,  byte(p & 0xff));
    outportb(FDC_DMA_S_BASE,  byte((p >> 8) & 0xff));
    outportb(FDC_DMA_S_COUNT, byte(size & 0xff));
    outportb(FDC_DMA_S_COUNT, byte(size >>8));
    outportb(FDC_DMA_PAGE2  , byte((p >>16)&0xFF));

    exit_kernel_lock_mode();

    startDMA();
    return;
}

/*!
    \brief disk read

    \param track  track
    \param head   head
    \param sector sector

    \author HigePon
    \date   create:2003/02/15 update:
*/
bool FDCDriver::read(byte track, byte head, byte sector) {

    byte command[] = {FDC_COMMAND_READ
                   , (head & 1) << 2
                   , track
                   , head
                   , sector
                   , 0x02
                   , 0x7E//0x12 for Vmware
                   , 0x01//0x1b
                   , 0xFF//0x00
                   };

    if (!seek(track)) {
        info(ERROR, "read#seek:error");
        return false;
    }

    setupDMARead(512);

    interrupt_ = false;
    if (!sendCommand(command, sizeof(command))) {

        info(ERROR, "read#send command:error\n");
        return false;
    }

    waitInterrupt();

    //    delay(30000);

    for (int i = 0; i < 7; i++) {

        results_[i] = getResult();
    }

    stopDMA();
    //  g_console->printf("status=%x", results_[0]);
    //    g_console->printf("%s", ((results_[0] & 0xC0) != 0x00) ? "true":"false");

    if ((results_[0] & 0xC0) != 0x00) return false;

    return true;
}

/*!
    \brief disk write

    \param track  track
    \param head   head
    \param sector sector

    \author HigePon
    \date   create:2003/02/15 update:2003/09/20
*/
bool FDCDriver::write(byte track, byte head, byte sector) {

    byte command[] = {0xC5//FDC_COMMAND_WRITE
                   , (head & 1) << 2
                   , track
                   , head
                   , sector
                   , 0x02
                   , 0x12
                   , 0x1b
                   , 0x00
                   };
    setupDMAWrite(512);

    seek(track);

    interrupt_ = false;
    sendCommand(command, sizeof(command));
    waitInterrupt();

    info(DEV_NOTICE, "write:after waitInterrupt\n");

    stopDMA();

    for (int i = 0; i < 7; i++) {
        results_[i] = getResult();
    }

    if ((results_[0] & 0xC0) != 0x00) return false;

    return true;
}

/*!
    \brief disk read

    \param lba    logical block address
    \param buf    read result buffer 512byte

    \author HigePon
    \date   create:2003/02/15 update:2003/09/20
*/
bool FDCDriver::read(dword lba, byte* buf) {

    info(DEV_WARNING, "read start \n");

    byte track, head, sector;

    lbaToTHS(lba, track, head, sector);

    //    info(DEBUG, "read lba=%d", lba);
    //    info(DEBUG, "[t h s]=[%d, %d, %d]\n", track, head, sector);

    /* read. if error, retry 10 times */
    for (int i = 0; i < 10; i++) {

        info(DEV_WARNING, "read %d times \n", i);


        if (read(track, head, sector)) {

            memcpy(buf, dmabuff_, 512);
            return true;
        }
    }

    return false;
}

/*!
    \brief disk write

    \param lba    logical block address
    \param buf    write result buffer 512byte

    \author HigePon
    \date   create:2003/02/15 update:2003/09/20
*/
bool FDCDriver::write(dword lba, byte* buf) {

    byte track, head, sector;

    lbaToTHS(lba, track, head, sector);

    info(DEBUG, "write lba=%d", lba);
    info(DEBUG, "[t h s]=[%d, %d, %d]\n", track, head, sector);

    memcpy(dmabuff_, buf,  512);

    /* write. if error, retry 10 times */
    for (int i = 0; i < 10; i++) {
        if (write(track, head, sector)) return true;
    }

    return false;
}

/*!
    \brief disk read

    \param lba    logical block address
    \param track  track
    \param head   head
    \param sector sector

    \author HigePon
    \date   create:2003/02/15 update:2003/09/20
*/
void FDCDriver::lbaToTHS(int lba, byte& track, byte& head, byte& sector) {

    track = lba / (2 * 18);
    head = (lba / 18) % 2;
    sector = 1 + lba % 18;
    return;
}
