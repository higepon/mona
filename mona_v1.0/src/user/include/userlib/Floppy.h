#ifndef _MONA_USERLIB_FLOPPY_
#define _MONA_USERLIB_FLOPPY_

#include <types.h>

/*----------------------------------------------------------------------
    Device
----------------------------------------------------------------------*/
interface Device {

  public:
    virtual int open()         = 0;
    virtual int close()        = 0;
    virtual int ioctl(void* p) = 0;
};

/*----------------------------------------------------------------------
    StorageDevice
----------------------------------------------------------------------*/
interface StorageDevice : public Device {

  public:
    virtual dword getBlockSize() const = 0;
    virtual int read(dword lba,  byte* buf, dword blocknum)  = 0;
    virtual int write(dword lba, byte* buf, dword blocknum)  = 0;
};

/*----------------------------------------------------------------------
    Floppy
----------------------------------------------------------------------*/
class Floppy : public StorageDevice {

  public:
    Floppy(int device);
    virtual ~Floppy();

  public:
    virtual int open();
    virtual int close();
    virtual int read(dword lba,  byte* buf, dword blocknum);
    virtual int write(dword lba, byte* buf, dword blocknum);
    virtual bool diskChanged();
    virtual int ioctl(void* p);

    inline virtual dword getBlockSize() const {
        return BLOCK_SIZE;
    }

  public:
    static const dword BLOCK_SIZE = 512;
    static const int FLOPPY_1     = 0;
};


#endif
