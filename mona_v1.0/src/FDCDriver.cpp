/*!
    \file FDCDriver.cpp
    \brief Floppy Disk Controller driver (class FDCDriver)

    \author syn
    \version $Revision$
    \date create:2002/10/14 update $Date$
*/

#include<monaVga.h>
#include<monaIo.h>
#include<monaOperator.h>
#include<FDCDriver.h>

#define MAX_COUNT               (1000000)
#define DMA_BUFF_SIZE           (4096)

/* ==== FDC ports ==== */
#define PORT_BASE_1             (0x3F0)
#define PORT_SRA_1              (PORT_BASE_1 +0)
#define PORT_SRB_1              (PORT_BASE_1 +1) 
#define PORT_DOR_1              (PORT_BASE_1 +2)
#define PORT_TDR_1              (PORT_BASE_1 +3)
#define PORT_MSR_1              (PORT_BASE_1 +4)
#define PORT_DSR_1              (PORT_BASE_1 +4)
#define PORT_FIFO_1             (PORT_BASE_1 +5)
#define PORT_DIR_1              (PORT_BASE_1 +7)
#define PORT_CCR_1              (PORT_BASE_1 +7)

/* ==== DMA ports ==== */
#define PORT_SDMA_BASE_C2   (0x04) /* Slave DMA BaseAddress (Channel 2) */
#define PORT_SDMA_COUNT_C2	(0x05) /* Slave DMA Count (Channel 2) */
#define PORT_SDMA_CMD	(0x08)
#define PORT_SDMA_SINGLE_MASK	(0x0A)
#define PORT_SDMA_MODE	(0x0B)
#define PORT_SDMA_CLEAR_BYTE	(0x0C)
#define PORT_SDMA_CLEAR	(0x0D)
#define PORT_DMA_PAGE_C2	(0x81)
#define PORT_MDMA_CMD	(0xD0)
#define PORT_MDMA_SINGLE_MASK	(0xD4)
#define PORT_MDMA_MODE	(0xD6)
#define PORT_MDMA_CLEAR	(0xDA)

/* Global Variables */
FDCDriver* gFDCDriver1 = 0;
FDCDriver* gFDCDriver2 = 0;

/*!
    \brief constructor

    \param n FDC select (0 or 1)
    \author syn

    \date create:2002/10/14 update:2002/10/14
*/
FDCDriver::FDCDriver(int n)
{
    _sys_printf("FDCDriver(%d)\n", n);
    initialize();
}

/*!
    \brief private destructor
    \author syn
    \date create:2002/10/14 update:2002/10/14
*/
FDCDriver::~FDCDriver()
{
    /* Do nothing */
}

/*!
    \brief initialize FDC
    \author syn
    \date create:2002/10/14 update:2002/10/31
*/
int FDCDriver::initialize()
{
    unsigned char cmd[3];

    _sys_printf("FDCDriver::initialize\n");

    /* Allocate DMA-Buffer */
    dma_buff = (unsigned char*)malloc(DMA_BUFF_SIZE);
    _sys_printf("dma_buff=%d\n", long(dma_buff));

    /* Initialize DMAC */
    outportb(PORT_MDMA_CLEAR, 0);  /* reset master DMAC */
    inportb(0x80);
    outportb(PORT_SDMA_CLEAR, 0);  /* reset slave DMAC */
    inportb(0x80);
    outportb(PORT_MDMA_CMD, 0x00);
    inportb(0x80);
    outportb(PORT_SDMA_CMD, 0x00);
    inportb(0x80);
    outportb(PORT_MDMA_MODE, 0xC0);
    inportb(0x80);
    outportb(PORT_SDMA_MODE, 0x46);
    inportb(0x80);
    outportb(PORT_MDMA_SINGLE_MASK, 0);

    /* Reset FDC */
    outportb(PORT_DSR_1, 0x00);
    inportb(0x80);
    outportb(PORT_CCR_1, 0x00);
    inportb(0x80);
    outportb(PORT_DOR_1, 0x08);
    inportb(0x80);
    outportb(PORT_DOR_1, 0x0C); /* Drive 0 Enable DMA */
    inportb(0x80);

    /* Specify FDC */
    cmd[0] = 0x03;
    cmd[1] = 0xC1; /* SRT = 4m[sec], HUT = 16m[sec] */
    cmd[2] = 0x10; /* HLT = 16m[sec], Use DMA */
    if(command(cmd, 3) != 0){
        _sys_printf("FDCDriver::initialize:command fail\n");
        return 0;
    }

	motor(1);
    recalibrate(); 
    motor(0);

    return 0;
}

/*!
    \brief motor on/off

    \param on: 0=off, other=on
    \result constant 0

    \author syn
    \date create:2002/10/14 update:2002/10/14
*/
int FDCDriver::motor(int on)
{
    if(on){
        outportb(PORT_DOR_1, 0x1C);    /* Drive0 Enable DMA, Motor on */
    }else{
        outportb(PORT_DOR_1, 0x0C);    /* Drive0 Enable DMA, Motor off */
    } 
    
    return 0;
}

/*!
    \brief Do recaribrate

    \result constant 0
    
    \author syn
    \date create:2002/10/31 update:2002/10/31
*/
int FDCDriver::recalibrate()
{
    unsigned char cmd[2];

    cmd[0] = 0x07;
    cmd[1] = 0x00; /* Head1, drive 0*/
 
    interrupt = false; 
    if(command(cmd, sizeof(cmd)) != 0){
        _sys_printf("FDCDriver::recalibrate:command fail\n");
        return 1;
    }

    while(!wait_interrupt()){
    }
    sense_interrupt();
    
    return 0; 
}

/*!
    \brief seek header

    \param head select head (0 or 1)
    \param track specify track
    \result if success, return 0

    \author syn
    \date create:2002/10/14 update:2002/10/14
*/
int FDCDriver::seek(const unsigned int head, const unsigned int track)
{
    unsigned char cmd[3];

    /* motor on */ 
    motor(1);
   
    /* create seek comand */
    cmd[0] = 0x0F;  /* seek */
    cmd[1] = 0x00;  /* drive 0, header 0 */
    cmd[2] = track;
 
    /* send seek command */
    interrupt = false;
    if(command(cmd, sizeof(cmd)) != 0){
        _sys_printf("FDCDriver::seek:command fail\n");
        return 1;
    }

    while(!wait_interrupt()){
    }
    sense_interrupt();

    return 0;
}

/*!
    \brief read from sector

    \param start_sector start sector
    \param num_sector number of read sector
    \param buff read buffer
    \return if succeed, return 0  
   
    \author syn
    \date create:2002/10/14 update:2002/10/20
*/
int FDCDriver::readSector(const unsigned long start_sector, const unsigned long num_sector, unsigned char buff[])
{
    int h, c, r;
    unsigned long sector;
    unsigned long i;
    int result;

    motor(1);
    sector = start_sector;
    for(i = 0; i < num_sector; i++){
        sector_to_hcr(sector, h, c, r); 
        result = readSector(h, c, r, &buff[i*512]);
        if(result != 0){
            return result;
        }
        sector++;
    }
    motor(0);

    return result;
}

/*!
    \brief read from sector 

    \param h head (normally, 0 or 1)
    \param c cylinder in head
    \param r sector in cylinder
    \param num_sector number of read sector
    \param buff read buffer
    \return if succeed, return 0  
   
    \author syn
    \date create:2002/10/20 update:2002/10/31
*/
int FDCDriver::readSector(const int h, const int c, const int r, const int num_sector, unsigned char buff[])
{
    _sys_printf("FDCDriver::readSector:sorry, not implemented yet\n");
    return 0;
}

/*!
    \brief read from a sector

    \param h head (normally, 0 or 1)
    \param c cylinder in head
    \param r sector in cylinder
    \param buff read buffer
    \return if succeed, return 0  
   
    \author syn
    \date create:2002/10/20 update:2002/10/31
*/
int FDCDriver::readSector(const int h, const int c, const int r, unsigned char buff[])
{
    unsigned char cmd[9];
    unsigned char result[7];
    unsigned long i;

    _sys_printf("FDCDriver::read_sector:h=%d,c=%d,r=%d\n", h, c, r);
    if(seek(h, c) != 0){
         _sys_printf("FDCDriver::read_sector:seek fail\n");
         goto err;
    }

    /* build command */
    cmd[0] = 0xE6;     /* Single sector read */
    if(h){
        cmd[1] = 0x04; /* Drive 0, head 1 */
    }else{
        cmd[1] = 0x00; /* Drive 0, head 0 */
    }
    cmd[2] = c&0xFF;
    cmd[3] = h&0xFF;
    cmd[4] = r&0xFF;
    cmd[5] = 2;        /* 512byte (= 128*(2^2) byte) */
    cmd[6] = 0x12;       /* end track */
    cmd[7] = 0x1B;     /* For 1.44M discket */
    cmd[8] = 0xFF;     /* DTL */

    /* prepare dma */
    dma_read(512);

    /* send command */
    interrupt = false;
    if(command(cmd, 9) != 0){
        _sys_printf("FDCDriver::readSector:command fail\n");
        goto err;
    }

    while(!wait_interrupt()){
    }
    _sys_printf("-\n");
    if(get_result(result, sizeof(result)) != 0){
        _sys_printf("FDCDriver::readSector:failed get_result\n");
        goto err;
    }
    _sys_printf("FDCDrive::readSector:result = ");
    for(i = 0; i < sizeof(result); i++){
        _sys_printf("[%x] ", result[i]);
    }
    _sys_printf("\n");
    
    for(i = 0; i < 512; i++){
        buff[i] = dma_buff[i];
    }

    return 0;

err:
   return 1;
}

/*!
    \brief write to sector

    \param start_sector start sector
    \param num_sector number of read sector
    \param buff write data
    \return if succeed, return 0  
   
    \author syn
    \date create:2002/10/14 update:2002/10/14
*/
int FDCDriver::writeSector(const unsigned long start_sector, const unsigned long num_sector, unsigned char buff[])
{
    unsigned int i;
    int h, c, r;
    int result;
    unsigned long sector;
  
    motor(1); 
    sector = start_sector;
    for(i = 0; i < num_sector; i++){
        sector_to_hcr(sector, h, c, r); 
        result = writeSector(h, c, r, &buff[i*512]);
        if(result != 0){
            motor(0);
            return result;
        }
        sector++;
    }

    motor(1); 
    return 0;
}

/*!
    \brief write to sector

    \param h head (normally, 0 or 1)
    \param c cylinder in head
    \param r sector in cylinder
    \param buff read buffer
    \return if succeed, return 0  

    \author syn
    \date create:2002/11/02 update:2002/11/02
*/
int FDCDriver::writeSector(const int h, const int c, const int r, unsigned char buff[])
{
    unsigned char cmd[9];
    unsigned char result[7];

    _sys_printf("FDCDriver::writeSector:h=%d,c=%d,r=%d\n", h, c, r);
    if(seek(h, c) != 0){
        _sys_printf("FDCDriver::writeSector:seek failed\n");
        goto err;
    }

    /* build command */
    cmd[0] = 0xC5;    /* single sector write */
    if(h){
        cmd[1] = 0x04; /* Drive 0, head 1 */
    }else{
        cmd[1] = 0x00; /* Drive 0, head 0 */
    }
    cmd[2] = c&0xFF;
    cmd[3] = h&0xFF;
    cmd[4] = r&0xFF;
    cmd[5] = 2;        /* 512byte (= 128*(2^2) byte) */
    cmd[6] = 0x12;       /* end track */
    cmd[7] = 0x1B;     /* For 1.44M discket */
    cmd[8] = 0xFF;     /* DTL */

    /* copy data to DMA buffer */
    for(int i = 0; i < 512; i++){
        dma_buff[i] = buff[i];
    }

    /* prepare dma */
    dma_write(512);

    /* send command */
    interrupt = false;
    if(command(cmd, 9) != 0){
        _sys_printf("FDCDriver::writeSector:command fail\n");
        goto err;
    }

    while(!wait_interrupt()){
    }
    _sys_printf("-\n");
    if(get_result(result, sizeof(result)) != 0){
        _sys_printf("FDCDriver::writeSector:failed get_result\n");
        goto err;
    }

    return 0;

err:
    return 1;
}

/*!
    \brief interrupt handler (called by monaIHandler.cpp:fdcHandler)

    \author syn
    \date create:2002/10/14 update:2002/10/31
*/
void FDCDriver::interruptHandler()
{
    _sys_printf("FDCDriver::interrupt_handler\n");
    interrupt = true;
}

/*!
    \brief wait MSR redy

    \param mask mask of MSR
    \param pat excepted pattern
    \result 0:OK other NG(maybe timeout)

    \author syn
    \date create:2002/10/15 update:2002/10/20
*/
int FDCDriver::wait_MSR(const unsigned char mask, const unsigned char pat)
{
    unsigned long count;
    unsigned char msr;
    unsigned char data;

    for(count = 0; count < MAX_COUNT; count++){
        msr = inportb(PORT_MSR_1);
        if((msr&mask) == pat){
            return 0;
        }
        if((msr&0x40) != 0){
             data = inportb(PORT_FIFO_1);
             _sys_printf("FDCDriver::wait_MSR:data=%d\n", data);
        }
    }

    _sys_printf("FDCDriver::wait_MSR:FDC Timeout(MSR=%d)\n", msr);
    return 1;
}

/*!
    \brief send SENSE INTERRUPT command to FDC

    \result 0:OK other NG(maybe timeout)

    \author syn
    \date create:2002/10/16 update:2002/10/31
*/
int FDCDriver::sense_interrupt()
{
    unsigned char cmd[1];
    unsigned char result[2];

    cmd[0] = 0x08;
    if(command(cmd, 1) != 0){
        _sys_printf("FDCDriver::sense_interrupt:command failed\n");
        return 1;
    }

    get_result(result, 2);
    _sys_printf("FDCDrive::sense_interrupt:result = [%x] [%x]\n", result[0], result[1]);
    
    /* decode result */
    /* ============================= */
    /* ==== NOT INPLEMENTED YET ==== */
    /* ============================= */

    return 0;
}

/*!
    \brief send command to FDC

    \param cmd command buffer
    \param len command length (in bytes) 
    \result 0:OK other NG(maybe timeout)

    \author syn
    \date create:2002/10/15 update:2002/10/15
*/
int FDCDriver::command(const unsigned char cmd[], const int len)
{
    int n;

    if(wait_MSR(0xF0, 0x80) != 0){
        _sys_printf("FDCDriver::command:MSR Timeout\n");
        return 1;
    }

    for(n = 0; n < len; n++){
        if(wait_MSR(0xC0, 0x80) != 0){
            _sys_printf("FDCDriver::command:MSR Timeout(n=%d)\n", n);
            return 1;
        }
        outportb(PORT_FIFO_1, cmd[n]);
    }

    return 0;
}

/*!
    \brief retreave result code from FDC

    \param r result code buffer
    \param n result code buffer's length [byte]
    \result 0:OK other NG(maybe timeout)

    \author syn
    \date create:2002/10/15 update:2002/10/31
*/
int FDCDriver::get_result(unsigned char r[], int n)
{
    int index = 0;
    unsigned char msr;
    unsigned char result;
    long count;

    for(count = 0; count < MAX_COUNT; count++){
        msr = inportb(PORT_MSR_1);
        if((msr & 0xC0) == 0xC0){
            break;
        }
        inportb(0x80); /* delay */
    }
    if(count == MAX_COUNT){
        _sys_printf("FDCDriver::get_result:timeout(msr=%d)\n", msr);
        return 1;
    }

    do{
        inportb(0x80); /* delay */ 
        result = inportb(PORT_FIFO_1);
        if(index < n){
            r[index++] = result;
        }
        for(count = 0; count < MAX_COUNT; count++){
            msr = inportb(PORT_MSR_1);
            if((msr & 0x80) == 0x80){
                break;
            }
        }
        if(count == MAX_COUNT){
            _sys_printf("FDCDriver::get_result:timeout2(msr=%d)\n", msr);
            return 0;
        }
    }while(msr &0x40);

    return 0;
}

/*!
    \brief wait interrupt raised

    \return if interrupt raised, return true

    \author syn
    \date create:2002/10/16 update:2002/10/16
*/
bool FDCDriver::wait_interrupt()
{
    return interrupt;
}

/*!
    \brief convert sector to head, cylinder, sector format

    \param s input sector
    \param h outout head
    \param c outout cylinder
    \param r output sector

    \author syn
    \date create:2002/11/02 update:2002/11/02
*/
void FDCDriver::sector_to_hcr(const unsigned long s, int& h, int& c, int& r)
{
    h = s/(80*18);   /* head */
    c = s -h;
    c /= 18;         /* cylinder */
    r = s%18;        /* sector */
}

/*!
    \brief set DMA to read mode

    \param len read length [byte]
    \return constant 0

    \autor syn
    \date create:2002/10/20 update:2002/10/31
*/
int FDCDriver::dma_read(const unsigned long len)
{
    unsigned long p = (unsigned long)dma_buff;

    /* setup DMA registor */
    asm("cli");
    outportb(PORT_SDMA_SINGLE_MASK, 0x06); /* mask channel #2 */
    outportb(PORT_SDMA_MODE, 0x46);        /* fdc -> memory */
    outportb(PORT_SDMA_CLEAR_BYTE, 0);     /* clear flip-flop */
    outportb(PORT_SDMA_BASE_C2, p&0xFF);
    outportb(PORT_SDMA_BASE_C2, (p>>8)&0xFF);
    outportb(PORT_SDMA_COUNT_C2, len &0xFF);
    outportb(PORT_SDMA_COUNT_C2, (len >>8)&0xFF);
    outportb(PORT_DMA_PAGE_C2, (p >>16)&0xFF);
    asm("sti");

    /* and, go! */
    outportb(PORT_SDMA_SINGLE_MASK, 0x02);

    return 0;
}

/*!
    \brief set DMA to write mode

    \param len read length [byte]
    \return constant 0

    \autor syn
    \date create:2002/10/20 update:2002/10/31
*/
int FDCDriver::dma_write(const unsigned long len)
{
    unsigned long p = (unsigned long)dma_buff;

    /* setup DMA registor */
    asm("cli");
    outportb(PORT_SDMA_SINGLE_MASK, 0x06); /* mask channel #2 */
    outportb(PORT_SDMA_MODE, 0x4A);        /* memory -> fdc */
    outportb(PORT_SDMA_CLEAR_BYTE, 0);     /* clear flip-flop */
    outportb(PORT_SDMA_BASE_C2, p&0xFF);
    outportb(PORT_SDMA_BASE_C2, (p>>8)&0xFF);
    outportb(PORT_SDMA_COUNT_C2, len &0xFF);
    outportb(PORT_SDMA_COUNT_C2, (len >>8)&0xFF);
    outportb(PORT_DMA_PAGE_C2, (p >>16)&0xFF);
    asm("sti");

    /* and, go! */
    outportb(PORT_SDMA_SINGLE_MASK, 0x02);

    return 0;
}
