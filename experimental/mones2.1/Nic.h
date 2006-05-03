#ifndef _MONA_MONES_NIC_
#define _MONA_MONES_NIC_

#include <sys/types.h>
#include <monapi/io.h>

namespace mones {

class Nic
{
public:
    virtual int init()  = 0;
    virtual int probe() = 0;
    virtual void inputFrame() = 0;
    virtual void outputFrame(byte* packet, byte* macAddress, dword size, word protocolId) = 0;
	virtual int interrupt() =0;

    virtual dword getFrameBufferSize() = 0;
    virtual void getFrameBuffer(byte* buffer, dword size) = 0;
    virtual void getMacAddress(byte* dest);
    virtual byte getIRQ() const = 0;
    virtual int getIOBase() const = 0;
    virtual void setIRQ(byte irq) = 0;
    virtual void setIOBase(int iobase) = 0;

    virtual void enableNetwork() {monapi_set_irq(this->getIRQ(), MONAPI_TRUE, MONAPI_TRUE);}
    virtual void disableNetwork() {monapi_set_irq(this->getIRQ(), MONAPI_FALSE, MONAPI_TRUE);}
	enum{
        RX_INT     =0x0004,
        TX_INT     =0x0002,
        ER_INT     =0x0001,
	};
    //void pop_frame_list
};

};
#endif
