/*!
    \file FDCDriver.h
    \brief Floppy Disk Controller driver

    \author syn
    \version $Revision$
    \date create:2002/10/14 update:$Date$
*/

#ifndef _HIGEPOS_FDCDRIVER_
#define _HIGEPOS_FDCDRIVER_

/*!
    Floppy Disk Controller class
*/
class FDCDriver{
  public:
    FDCDriver(const int n);
	~FDCDriver();

  public:
    int seek(const unsigned int head, const unsigned int track);
    int readSector(const unsigned long start_sector, const unsigned long num_sector, unsigned char buff[]);
    int readSector(const int h, const int c, const int r, const int num_sector, unsigned char buff[]);
    int readSector(const int h, const int c, const int r, unsigned char buff[]);
    int writeSector(const unsigned long start_sector, const unsigned long num_sector, unsigned char buff[]);
    int writeSector(const int h, const int c, const int r, unsigned char buff[]);
	void interruptHandler();
  
  protected:
    int initialize();
    int motor(int on);
    int wait_MSR(const unsigned char mask, const unsigned char pat);
    int sense_interrupt();
    int recalibrate();
    int command(const unsigned char cmd[], const int n);
    bool wait_interrupt();
    int dma_read(const unsigned long len);
    int dma_write(const unsigned long len);
    void sector_to_hcr(const unsigned long sec, int& h, int& c, int& r);
    static int get_result(unsigned char r[], int n);
  
  protected:
    bool interrupt;
    unsigned char* dma_buff;
};

extern FDCDriver* gFDCDriver1;
extern FDCDriver* gFDCDriver2;
#endif
