/* IDE Driver test */
#include<types.h>
#include<VirtualConsole.h>
#include<DiskDriver.h>

#ifndef _MONA_MIDEDRIVER_MJT
#define _MONA_MIDEDRIVER_MJT

/*!
    Floppy Disk Controller class
*/
class IDEDriver : public DiskDriver {
  public:
    IDEDriver(VirtualConsole* console,unsigned int port,unsigned int drive);
    ~IDEDriver();

  public:
    bool read(int lba, byte* buf);
    bool write(int lba, byte* buf);
  
  private:
    bool sendcmd(int cmd,short int *bfr,int bfrsize /* ignored */);
    bool senddrive(int drive);
    bool waithdc(unsigned long timeout);
    bool waitdata(unsigned long timeout);
    bool waitready(unsigned long timeout);
    void initilize();
  private:
    byte version_;
    static unsigned int control_;
    static unsigned int data_;
    static unsigned int error_;
    static unsigned int status_;
    static unsigned int sector_;
    static unsigned int count_;
    static unsigned int cylinderL_;
    static unsigned int cylinderH_;
    static unsigned int head_;
    static unsigned int status2_;
    static unsigned int driveaddr_;
    static unsigned int drive_;
    
    static byte results_[10];
    static int resultsLength_;
    static bool interrupt_;
    static VirtualConsole* console_;
    static byte* dmabuff_;
};

extern IDEDriver* gIDEDriver;

#endif
