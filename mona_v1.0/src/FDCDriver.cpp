/*!
    \file  FDCDriver.cpp
    \brief class Floppy Disk Controller for MultiTask

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/02/07 update:$Date$
*/

#include<FDCDriver.h>
#include<kernel.h>
#include<operator.h>
#include<io.h>
#include<global.h>

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
#define FDC_RETRY_MAX 6000000

/* delay */
#define delay() inportb(0x80);

#define FDC_DMA_BUFF_SIZE 512

bool            FDCDriver::interrupt_ ;
byte*           FDCDriver::dmabuff_;
byte            FDCDriver::results_[10];
VirtualConsole* FDCDriver::console_;
int             FDCDriver::resultsLength_;

/*!
    \brief Constructer

    \param  console message out console
    \author HigePon
    \date   create:2003/02/03 update:
*/
FDCDriver::FDCDriver(VirtualConsole* console) {

    /* set console */
    console_ = console;

    initilize();
}

/*!
    \brief Destructer

    \author HigePon
    \date   create:2003/02/03 update:
*/
FDCDriver::~FDCDriver() {

    motor(OFF);
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

    console_->printf("dma buff_=[%x]=[%dkb]\n", (dword)dmabuff_, ((dword)dmabuff_/1024));

    /* dma buff should be 64kb < dma buff < 16Mb */
    if (!dmabuff_ || (dword)dmabuff_ < 64 * 1024 || (dword)dmabuff_  + FDC_DMA_BUFF_SIZE > 16 * 1024 * 1024) {
        panic("dma buff allocate error");
    }

    /* setup DMAC */
    outportb(0xda, 0x00);
    delay();
    outportb(0x0d, 0x00);
    delay();
    outportb(0xd0, 0x00);
    delay();
    outportb(0x08, 0x00);
    delay();
    outportb(0xd6, 0xc0);
    delay();
    outportb(0x0b, 0x46);
    delay();
    outportb(0xd4, 0x00);
    delay();

    /* reset drive */
    outportb(FDC_DOR_PRIMARY, FDC_DOR_RESET);

    delay();
    outportb(FDC_CCR_PRIMARY, 0);

    interrupt_ = false;
    motor(ON);
    while(!waitInterrupt());

    /* specify */
    if (!sendCommand(specifyCommand, sizeof(specifyCommand))) {

        g_console->printf("Specify command failed\n");
        motor(OFF);
        return;
    }

    recalibrate();
    recalibrate(); /* 2nd recalibrate occurs 3interrupts on VPC and 2 on Bochs */

    g_console->printf("recalibrate done\n");

    memset(dmabuff_, 0x10, 512);
    dmabuff_[0] = 'M';
    dmabuff_[1] = 'o';
    dmabuff_[2] = 'n';
    dmabuff_[3] = 'a';
    dmabuff_[4] = '\0';

    g_console->printf("Writing [%s]\n", dmabuff_);
    if (!write(0, 0, 150)) {

        g_console->printf("write failed\n");
        motor(OFF);
        return;
    }

    recalibrate();
    recalibrate();
    if (!read(0, 0, 150)) {

        g_console->printf("read failed\n");
        motor(OFF);
        return;
    }
    g_console->printf("reading result is %s\n", dmabuff_);
    motor(OFF);
    return;
}

/*!
    \brief print status of FDC(MSR)

    \param  str message
    \author HigePon
    \date   create:2003/02/10 update:
*/
void FDCDriver::printStatus(const char* str) const {

    byte msr = inportb(FDC_MSR_PRIMARY);
    printStatus(msr, str);
    return;
}

/*!
    \brief print status of FDC(MSR)

    \param  main status register
    \param  str message
    \author HigePon
    \date   create:2003/02/10 update:
*/
void FDCDriver::printStatus(const byte msr, const char* str) const {

    console_->printf("data reg |data flow|DMA|BUSY| D | C | B | A |int|\n");
    console_->printf("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n"
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
    return;
}

/*!
    \brief interrupt handler

    \author HigePon
    \date   create:2003/02/10 update:
*/
void FDCDriver::interrupt() {

    console_->printf("\ninterrpt:");

    interrupt_ = true;
}

/*!
    \brief wait interrupt

    \return true:interrupt
    \author HigePon
    \date   create:2003/02/10 update:
*/
bool FDCDriver::waitInterrupt() {

    return interrupt_;
}

/*!
    \brief print status of FDC

    \param  on ON/OFF
    \author HigePon
    \date   create:2003/02/10 update:
*/
void FDCDriver::motor(bool on) {

    if (on) outportb(FDC_DOR_PRIMARY, FDC_START_MOTOR);
    else outportb(FDC_DOR_PRIMARY, FDC_STOP_MOTOR);
    return;
}

/*!
    \brief send command to FDC

    \param  command array of command
    \param  length  length of command
    \author HigePon
    \date   create:2003/02/16 update:2003/02/19
*/
bool FDCDriver::sendCommand(const byte* command, const byte length) {

    /* send command */
    for (int i = 0; i < length; i++) {

        /* expected condition is ready & date I/O to Controller */
        if (!checkMSR(FDC_MRQ_READY, FDC_MRQ_READY)) {

            console_->printf("FDCDriver#sendCommand: timeout command[%d]\n", i);
            return false;
        }

        /* debug show command */
        //        console_->printf("command[%x] ", (int)command[i]);

        /* send command */
        outportb(FDC_DR_PRIMARY, command[i]);
    }
    return true;
}

/*!
    \brief recalibrate

    \return true OK/false command fail
    \author HigePon
    \date   create:2003/02/10 update:
*/
bool FDCDriver::recalibrate() {

    byte command[] = {0x07, 0x00}; /* recalibrate */

    interrupt_ = false;
    if(!sendCommand(command, sizeof(command))){

        console_->printf("FDCDriver#recalibrate:command fail\n");
        return false;
    }

    // comment out for bochs x86 emulator
    //    while(!waitInterrupt());
    senseInterrupt();
    return true;
}

/*!
    \brief wait until FDC is ready.

    \param  expectedcondition
    \return true ready/false time out
    \author HigePon
    \date   create:2003/02/10 update:
*/
bool FDCDriver::checkMSR(byte expectedCondition, byte mask) {

    bool isOK   = false;
    byte status = 0;

    /* check whether FDC is expected condition */
    for (dword i = 0; i < FDC_RETRY_MAX; i++) {

       status = inportb(FDC_MSR_PRIMARY);
       isOK = (status & mask) == expectedCondition;


       if (isOK) return true;
       if (i == FDC_RETRY_MAX - 2)  {
#ifdef DEBUG_FDC
           g_console->printf("is oK=[%d] status=[%x] masked=[%x]", (int)isOK, (int)status, (int)(status&mask));
#endif
       }
    }

#ifdef DEBUG_FDC
    /* time out */
    console_->printf("FDCDriver#checkMSR expectedCondition=[%x] result=[%x] masked=[%x]\n"
                   , (int)expectedCondition, (int)inportb(FDC_MSR_PRIMARY), (int)(status & mask));
#endif

    return false;
}

/*!
    \brief wait until FDC is ready.

    \param  expectedcondition
    \return true ready/false time out
    \author HigePon
    \date   create:2003/02/10 update:
*/
bool FDCDriver::checkMSR(byte expectedCondition) {

    return (checkMSR(expectedCondition, 0xffff));
}

/*!
    \brief seek

    \param  track
    \return true OK/false time out
    \author HigePon
    \date   create:2003/02/11 update:
*/
bool FDCDriver::seek(byte track) {

    byte command[] = {FDC_COMMAND_SEEK, 0, track};

    if(!sendCommand(command, sizeof(command))){

        console_->printf("FDCDriver#seek:command fail\n");
        return false;
    }

    if (!senseInterrupt()) {

        console_->printf("FDCDriver#seek:command fail\n");
        return false;
    }

#ifdef DEBUG_FDC
    printStatus("seek sense after");
#endif
    return true;
}

/*!
    \brief Sense Interrrupt Command

    \author HigePon
    \date   create:2003/02/13 update:
*/
bool FDCDriver::senseInterrupt() {

    byte command[] = {FDC_COMMAND_SENSE_INTERRUPT};


    if(!sendCommand(command, sizeof(command))){

        console_->printf("FDCDriver#senseInterrrupt:command fail\n");
        return false;
    }

    readResults();
    return true;
}

/*!
    \brief result phase

    \author HigePon
    \date   create:2003/02/13 update:
*/
void FDCDriver::readResults() {

    int i;
    byte msr = 0;
    resultsLength_ = 0;

    /* result phase */
    for (i = 0; i < 10; i++) {

        /* expected condition is ready & data I/O to CPU */
        if (!checkMSR(FDC_MRQ_READY, FDC_MRQ_READY)) {

            console_->printf("FDCDriver#readResults: timeout results_[%d]\n", i);
            break;
        }

        /* no result left */
        if (!(msr = inportb(FDC_MSR_PRIMARY) & FDC_DIO_TO_CPU)) break;

        /* get result */
        results_[i] = inportb(FDC_DR_PRIMARY);

    }
    resultsLength_ = i;

#ifdef DEBUG_FDC
    console_->printf("resultsLength_=%d\n", resultsLength_);

    /* debug show result */
    for (int j = 0; j < resultsLength_; j++) {

         console_->printf("result[%d] = %x ", j, (int)(results_[j]));
    }
    console_->printf("\n");
#endif

    return;
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
    \date   create:2003/02/15 update:
*/
void FDCDriver::setupDMARead(dword size) {

    dword p = (dword)dmabuff_;

    stopDMA();

    /* direction write */
    outportb(FDC_DMA_S_MR, 0x46);

    disableInterrupt();

    /* clear byte pointer */
    outportb(FDC_DMA_S_CBP, 0);
    outportb(FDC_DMA_S_BASE,  byte(p & 0xff));
    outportb(FDC_DMA_S_BASE,  byte((p >> 8) & 0xff));
    outportb(FDC_DMA_S_COUNT, byte(size & 0xff));
    outportb(FDC_DMA_S_COUNT, byte(size >>8));
    outportb(FDC_DMA_PAGE2  , byte((p >>16)&0xFF));

    enableInterrupt();

    startDMA();
    return;
}

/*!
    \brief setup dmac for write

    \author HigePon
    \date   create:2003/02/15 update:
*/
void FDCDriver::setupDMAWrite(dword size) {

    dword p = (dword)dmabuff_;

    stopDMA();

    /* direction read */
    outportb(FDC_DMA_S_MR, 0x4a);

    disableInterrupt();

    /* clear byte pointer */
    outportb(FDC_DMA_S_CBP, 0);
    outportb(FDC_DMA_S_BASE,  byte(p & 0xff));
    outportb(FDC_DMA_S_BASE,  byte((p >> 8) & 0xff));
    outportb(FDC_DMA_S_COUNT, byte(size & 0xff));
    outportb(FDC_DMA_S_COUNT, byte(size >>8));
    outportb(FDC_DMA_PAGE2  , byte((p >>16)&0xFF));

    enableInterrupt();

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
                   , 3
                   , 0x1b
                   , 0x00
                   };

    seek(track);

    setupDMARead(512);

    interrupt_ = false;
#ifdef DEBUG_FDC
    printStatus("before read command");
#endif

    sendCommand(command, sizeof(command));
    //    while(!waitInterrupt());
    stopDMA();

    readResults();
    return true;
}

/*!
    \brief writeID

    \author HigePon
    \date   create:2003/02/15 update:
*/
bool FDCDriver::writeID(byte track, byte head, byte data) {

    seek(track);


    return false;
}

/*!
    \brief print dma controler status

    \author HigePon
    \date   create:2003/02/15 update:
*/
void FDCDriver::printDMACStatus(const byte status, const char* str) const {

    console_->printf("ch3|ch2|ch1|ch0|ch3|ch2|ch1|ch0|\n");
    console_->printf("%s|%s|%s|%s|%s|%s|%s|%s|%s|\n"
              , status & 0x80 ? " Y ":" N "
              , status & 0x40 ? " Y ":" N "
              , status & 0x20 ? " Y ":" N "
              , status & 0x10 ? " Y ":" N "
              , status & 0x08 ? " Y ":" N "
              , status & 0x04 ? " Y ":" N "
              , status & 0x02 ? " Y ":" N "
              , status & 0x01 ? " Y ":" N "
              , str
    );
}

/*!
    \brief disk write

    \param track  track
    \param head   head
    \param sector sector

    \author HigePon
    \date   create:2003/02/15 update:
*/
bool FDCDriver::write(byte track, byte head, byte sector) {

    byte command[] = {0xC5//FDC_COMMAND_WRITE
                   , (head & 1) << 2
                   , track
                   , head
                   , sector
                   , 0x02
                   , sector
                   , 0x1b
                   , 0x00
                   };
    setupDMAWrite(512);
    seek(track);

    interrupt_ = false;
    sendCommand(command, sizeof(command));
    while(!waitInterrupt());
    stopDMA();

    readResults();
    return true;
}
