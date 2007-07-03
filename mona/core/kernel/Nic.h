#ifndef _MONA_MONES_NIC_
#define _MONA_MONES_NIC_

#include <sys/types.h>
#include <io.h>
extern "C" void monapi_set_irq(int irq, int enabled, int auto_ir2);
namespace mones {

class Ether
{
public:
#pragma pack(2)
    typedef struct{
        uint8_t  dstmac[6];   // 送信先 MAC ID
        uint8_t  srcmac[6];   // 送信元 MAC ID
        uint16_t    type;     // フレームタイプ Frame type(DIX) or frame length(IEEE)
        uint8_t   data[0x600];// Data
    } Frame;
#pragma pack(0)
    enum
    {
        ARP = 0x806,
        IP  = 0x800,
    };
};

class FrameNode : public Node
{
public:
    FrameNode() { frame = new Ether::Frame; }
    ~FrameNode() {}
    Ether::Frame* frame;
};

class Nic
{
public:
    virtual int init()  = 0;
    virtual int probe() = 0;
    virtual void inputFrame() = 0;
    virtual void outputFrame(uint8_t* packet, uint8_t* macAddress, uint32_t size, uint16_t protocolId) = 0;

    virtual uint32_t getFrameBufferSize() = 0;
    virtual void getFrameBuffer(uint8_t* buffer, uint32_t size) = 0;
    virtual void getMacAddress(uint8_t* dest);
    virtual uint8_t getIRQ() const = 0;
    virtual int getIOBase() const = 0;
    virtual void setIRQ(uint8_t irq) = 0;
    virtual void setIOBase(int iobase) = 0;

    virtual void enableNetwork() {monapi_set_irq(this->getIRQ(), 1, 1);}
    virtual void disableNetwork() {monapi_set_irq(this->getIRQ(), 0, 1);}

    //void pop_frame_list
};

};
#endif
