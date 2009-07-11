extern "C" {
#include <uip.h>
#include <uip_arp.h>
};

// ~/src/qemu-0.9.0/i386-softmmu/qemu 47-cdrom ~/mona/tool/mkimg/mona.iso -fda ~/mona/tool/mkimg/mona.img -boot d -soundhw pcspk -k ja -serial file:/tmp/qemu_mona.log -soundhw es1370 -net nic -net user -redir tcp:5555:10.0.2.15:80 -redir udp:5555:10.0.2.15:5555
// you need a patch for qemu, ask higepon.

//#include "ServerMessageLoop.h"
#include "monadev.h"
#include "NicServer.h"
#include <monapi.h>

using namespace mones;
static NicServer* server;
void NicListenLoop();
uint32_t nic_read(uint32_t nicThread, Ether::Frame* frame);
uint32_t nic_write(uint32_t nicThread, OutPacket* packet);

uint32_t nicThread;

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

extern void MessageLoop();
extern void* test();
extern void irc();
void thread_client()
{
//    MessageLoop();
//    test();
//    irc();
    return;
}

void thread_init()
{
    uint32_t id = 3;//(uint32_t)syscall_mthread_create((uint32_t)thread_client);
// comment out by higepon
//    syscall_mthread_join(id);
}

#ifndef NULL
#define NULL (void *)0
#endif /* NULL */
// int main2(int argc, char* argv[])
// {
//     uint32_t id = 3;//syscall_mthread_create((uint32_t)NicListenLoop);
//     if (MONAPI_FALSE == monapi_notify_server_start("MONITOR.BIN"))
//     {
//         exit(-1);
//     }

// // comment out by higepon
// //    syscall_mthread_join(id);

//     for (;;) {
//         if (server != NULL && server->isStarted()) {
//             break;
//         }
//         sleep(500);
//     }
//     nicThread = server->getThreadID();
//     u8_t i, arptimer;

//     /* Initialize the device driver. */
//     monadev_init();
//     thread_init();
//     /* Initialize the uIP TCP/IP stack. */
//     uip_init();
//     /* Initialize the HTTP server. */
//     httpd_init();


//     arptimer = 0;

//     while(1) {
//         /* Let the monadev network device driver read an entire IP packet
//            into the uip_buf. If it must wait for more than 0.5 seconds, it
//            will return with the return value 0. If so, we know that it is
//            time to call upon the uip_periodic(). Otherwise, the monadev has
//            received an IP packet that is to be processed by uIP. */
//         uip_len = monadev_read(receiver);
//         if(uip_len == 0) {
//             for(i = 0; i < UIP_CONNS; i++) {
//                 uip_periodic(i);
//                 /* If the above function invocation resulted in data that
//                    should be sent out on the network, the global variable
//                    uip_len is set to a value > 0. */
//                 if(uip_len > 0) {
//                     uip_arp_out();
//                     monadev_send();
//                 }
//             }

// #if UIP_UDP
//             for(i = 0; i < UIP_UDP_CONNS; i++) {
//                 uip_udp_periodic(i);
//                 /* If the above function invocation resulted in data that
//                    should be sent out on the network, the global variable
//                    uip_len is set to a value > 0. */
//                 if(uip_len > 0) {
//                     uip_arp_out();
//                     monadev_send();
//                 }
//             }
// #endif /* UIP_UDP */

//             /* Call the ARP timer function every 10 seconds. */
//             if(++arptimer == 20) {
//                 uip_arp_timer();
//                 arptimer = 0;
//             }

//         } else {
//             if(BUF->type == htons(UIP_ETHTYPE_IP)) {
//                 uip_arp_ipin();
//                 uip_input();
//                 /* If the above function invocation resulted in data that
//                    should be sent out on the network, the global variable
//                    uip_len is set to a value > 0. */
//                 if(uip_len > 0) {
//                     uip_arp_out();
//                     monadev_send();
//                 }
//             } else if(BUF->type == htons(UIP_ETHTYPE_ARP)) {
//                 uip_arp_arpin();
//                 /* If the above function invocation resulted in data that
//                    should be sent out on the network, the global variable
//                    uip_len is set to a value > 0. */
//                 if(uip_len > 0) {
//                     monadev_send();
//                 }
//             }
//         }

//     }
//     return 0;
// }
/*-----------------------------------------------------------------------------------*/
void
uip_log(char *m)
{
    _printf("uIP log message: %s\n", m);
}
/*-----------------------------------------------------------------------------------*/

void NicListenLoop()
{
    server = new NicServer;
    if (!server->initialize())
    {
        _printf("NicServer initialize failed\n");
        exit(1);
    }
    server->messageLoop();
}

uint32_t nic_read(uint32_t nicThread, Ether::Frame* frame)
{
    MessageInfo msg;
    if (MonAPI::Message::sendReceive(&msg, nicThread, MSG_FRAME_READ))
    {
        _printf("send error 1");
        return 1;
    }
    if (1 == msg.arg2) {
        return 1;
    }

    GetFrameFromSharedMemory(frame);
    return 0;
}

// caller should free() packet, after packet written
// not thread safe
uint32_t nic_write(uint32_t nicThread, OutPacket* packet)
{
    MessageInfo msg;
    if (MonAPI::Message::sendReceive(&msg, nicThread, MSG_FRAME_WRITE, (uint32_t)packet))
    {
        _printf("send error 1");
        return 1;
    }
    return 0;
}
#include <monapi.h>
#include <monapi/CString.h>
#include <monalibc.h>
#include <pci/Pci.h>
#include <monapi/io.h>

#include <monapi.h>


#define PCI_VENDOR_ID_REDHAT_QUMRANET    0x1af4
#define PCI_SUBVENDOR_ID_REDHAT_QUMRANET 0x1af4
#define PCI_SUBDEVICE_ID_QEMU            0x1100

#define PCI_DEVICE_ID_VIRTIO_NET         0x1000
#define PCI_DEVICE_ID_VIRTIO_BLOCK       0x1001
#define PCI_DEVICE_ID_VIRTIO_BALLOON     0x1002
#define PCI_DEVICE_ID_VIRTIO_CONSOLE     0x1003

/* A 32-bit r/o bitmask of the features supported by the host */
#define VIRTIO_PCI_HOST_FEATURES        0

/* A 32-bit r/w bitmask of features activated by the guest */
#define VIRTIO_PCI_GUEST_FEATURES       4

/* A 32-bit r/w PFN for the currently selected queue */
#define VIRTIO_PCI_QUEUE_PFN            8

/* A 16-bit r/o queue size for the currently selected queue */
#define VIRTIO_PCI_QUEUE_NUM            12

/* A 16-bit r/w queue selector */
#define VIRTIO_PCI_QUEUE_SEL            14

/* A 16-bit r/w queue notifier */
#define VIRTIO_PCI_QUEUE_NOTIFY         16

/* An 8-bit device status register.  */
#define VIRTIO_PCI_STATUS               18

#define VIRTIO_PCI_ISR                  19


#define VIRTIO_PCI_CONFIG               20
#define PAGE_SHIFT (12)
#define PAGE_SIZE  (1<<PAGE_SHIFT)
#define PAGE_MASK  (PAGE_SIZE-1)

/* Status byte for guest to report progress, and synchronize features. */
/* We have seen device and processed generic fields (VIRTIO_CONFIG_F_VIRTIO) */
#define VIRTIO_CONFIG_S_ACKNOWLEDGE     1
/* We have found a driver for the device. */
#define VIRTIO_CONFIG_S_DRIVER          2
/* Driver has used its parts of the config, and is happy */
#define VIRTIO_CONFIG_S_DRIVER_OK       4
/* We've given up on this device. */
#define VIRTIO_CONFIG_S_FAILED          0x80

#define MAX_QUEUE_NUM      (512)

#define VRING_DESC_F_NEXT  1
#define VRING_DESC_F_WRITE 2

#define VRING_AVAIL_F_NO_INTERRUPT 1

#define VRING_USED_F_NO_NOTIFY     1

typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

struct vring_desc
{
   u64 addr;
   u32 len;
   u16 flags;
   u16 next;
};

struct vring_avail
{
   u16 flags;
   u16 idx;
   u16 ring[0];
};

struct vring_used_elem
{
   u32 id;
   u32 len;
};

struct vring_used
{
   u16 flags;
   volatile u16 idx;
   struct vring_used_elem ring[];
};

struct vring {
   unsigned int num;
   struct vring_desc *desc;
   struct vring_avail *avail;
   struct vring_used *used;
};

struct virtio_net_hdr
{
#define VIRTIO_NET_HDR_F_NEEDS_CSUM     1       // Use csum_start, csum_offset
   uint8_t flags;
#define VIRTIO_NET_HDR_GSO_NONE         0       // Not a GSO frame
#define VIRTIO_NET_HDR_GSO_TCPV4        1       // GSO frame, IPv4 TCP (TSO)
/* FIXME: Do we need this?  If they said they can handle ECN, do they care? */
#define VIRTIO_NET_HDR_GSO_TCPV4_ECN    2       // GSO frame, IPv4 TCP w/ ECN
#define VIRTIO_NET_HDR_GSO_UDP          3       // GSO frame, IPv4 UDP (UFO)
#define VIRTIO_NET_HDR_GSO_TCPV6        4       // GSO frame, IPv6 TCP
#define VIRTIO_NET_HDR_GSO_ECN          0x80    // TCP has ECN set
   uint8_t gso_type;
   uint16_t hdr_len;
   uint16_t gso_size;
   uint16_t csum_start;
   uint16_t csum_offset;
};


#define vring_size(num) \
   (((((sizeof(struct vring_desc) * num) + \
      (sizeof(struct vring_avail) + sizeof(u16) * num)) \
         + PAGE_MASK) & ~PAGE_MASK) + \
    (sizeof(struct vring_used) + sizeof(struct vring_used_elem) * num))

// send Ether packet

// class Ether
// {
// public:
// #pragma pack(2)
//     typedef struct{
//         uint8_t  dstmac[6];   // 送信先 MAC ID
//         uint8_t  srcmac[6];   // 送信元 MAC ID
//         uint16_t    type;     // フレームタイプ Frame type(DIX) or frame length(IEEE)
//         uint8_t   data[0x600];// Data
//     } Frame;
// #pragma pack(0)
//     enum
//     {
//         ARP = 0x806,
//         IP  = 0x800,
//     };
// };

struct virtio_net_config
{
    /* The config defining mac address (6 bytes) */
    uint8_t mac[6];
    /* See VIRTIO_NET_F_STATUS and VIRTIO_NET_S_* above */
    uint16_t status;
} __attribute__((packed));

class Util
{
public:
    inline static uint32_t swapLong(uint32_t value)
    {
        return (value>>24)+((value>>8)&0xff00)+((value<<8)&0xff0000)+(value<<24);
    }

    inline static uint16_t swapShort(uint16_t value)
    {
        return (value>>8)+(value<<8);
    }

    inline static uint16_t get2uint8_t(uint8_t *buf, uint32_t offset)
    {
        uint16_t a;
        a = buf[offset++] << 8;
        return a | buf[offset];
    }

    inline static uint32_t ipAddressToUint32_t(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
    {
        return (d << 24) | (c << 16) | (b << 8) | a;
    }

    inline static MonAPI::CString ipAddressToCString(uint32_t a)
    {
        char buf[64];
        sprintf(buf, "%d.%d.%d.%d", a & 0xff, (a >> 8) & 0xff, (a >> 16) & 0xff, (a >> 24) & 0xff);
        return MonAPI::CString(buf);
    }

    inline static uint16_t calcCheckSum(uint32_t *data,int size)
    {
        union{
            unsigned long long u64;
            uint32_t            u32[2];
            uint16_t             u16[4];
        }sum;

        uint32_t tmp;


        sum.u64=0;
        for(;(uint32_t)size>=sizeof(uint32_t);size-=sizeof(uint32_t))
            sum.u64+=*data++;
        if(size>0)sum.u64+=*data&((1<<(size*8))-1);

        tmp=sum.u32[1];
        sum.u32[1]=0;
        sum.u64+=tmp;
        tmp=sum.u32[1];
        sum.u32[1]=0;
        sum.u32[0]+=tmp;

        tmp=sum.u16[1];
        sum.u16[1]=0;
        sum.u32[0]+=tmp;
        tmp=sum.u16[1];
        sum.u16[1]=0;
        sum.u16[0]+=tmp;

        return ~sum.u16[0];
    }

};

// class Arp
// {
// public:
//     typedef struct
//     {
//         uint16_t hardType  __attribute__((packed));
//         uint16_t protType  __attribute__((packed));
//         uint8_t  hardAddrLen;
//         uint8_t  protAddrLen;
//         uint16_t opeCode  __attribute__((packed));
//         uint8_t  srcMac[6];
//         uint32_t   srcIp  __attribute__((packed));
//         uint8_t  dstMac[6];
//         uint32_t   dstIp  __attribute__((packed));
//     } Header;

//     enum
//     {
//         HARD_TYPE_ETHER= 1,
//         PROTCOL_TYPE_IP=0x0800,
//         /* Operation code. */
//         OPE_CODE_ARP_REQ= 1,
//         OPE_CODE_ARP_REP= 2,
//         OPE_CODE_RARP_REQ=3,
//         OPE_CODE_RARP_REP=4,
//     };
// };


class Buffer {
public:
    Buffer(uintptr_t size) : size_(size + 1), pageNum_((size_ + PAGE_SIZE - 1) / PAGE_SIZE)
    {
        uintptr_t address = startAddress;
        bool ret = syscall_allocate_contiguous(address, pageNum_);
        if (!ret) {
            _printf("panic virtio buffer error\n");
            exit(-1);
        }
        startAddress += pageNum_ * PAGE_SIZE;
        data_ = (uint8_t*)address;
        memset(data_, 0, size_);
        data_[size_ - 1] = 0xcc;
    }

    void check(const char* file, int line) const
    {
        if (data_[size_ - 1] != 0xcc) {
            _printf("ASSERT %s:%d\n", file, line);
            exit(-1);
        }
    }

    uint8_t* data() const { return data_; }

    uintptr_t size() const { return size_; }

private:
    uintptr_t size_;
    int pageNum_;
    uint8_t* data_;
    static uintptr_t startAddress;
};

// class Icmp {
// public :
// #pragma pack(2)
//     typedef struct{
//         unsigned char  type;        /* ICMPタイプ。 */
//         unsigned char  code;        /* ICMPコード。 */
//         unsigned short chksum;      /* チェックサム。 */
//         char           data[32];     /* データ。 */
//     } Header;
// #pragma pack(0)
// };

// class UDP {
// public:
//  typedef struct{
//     uint16_t srcport;
//     uint16_t dstport;
//     short len;
//     uint16_t chksum;
//     char   data[0];
// } Header;
// };

// class IP {
// public:
//     enum {
//         ICMP = 0x01,
//         UDP = 0x11,
//     };
//     typedef struct{
//         uint8_t  verhead;  /* バージョン、ヘッダ長。 */
//         uint8_t  tos;      /* TOS. */
//         uint16_t len;       /* トータル長。 */
//         uint16_t id;        /* 識別番号。 */
//         uint16_t frag;      /* フラグ、フラグメントオフセット。 */
//         uint8_t  ttl;      /* Time to Live. */
//         uint8_t  prot;     /* プロトコル番号。 */
//         uint16_t chksum;    /* ヘッダチェックサム。 */
//         uint32_t srcip;        /* 送り元IP。 */
//         uint32_t dstip;        /* 宛先IP。 */
//         char     data[0];
//     } Header;
// };


uintptr_t Buffer::startAddress = 0x9E000000;

#define CHECK_BUFFER(buf) buf.check(__FILE__, __LINE__)


#define VIRT_LOG(...) printf("[%s:%d] ", __FILE__, __LINE__), printf(__VA_ARGS__), printf("\n")

// VirtioNet works weeeeeell
class VirtioNet
{
private:
    enum VringType
    {
        VRING_TYPE_READ = 0,
        VRING_TYPE_WRITE = 1
    };

    vring* readVring_;
    vring* writeVring_;
    int lastUsedIndexRead_;
    int lastUsedIndexWrite_;
    int irqLine_;
    uintptr_t baseAddress_;
    Ether::Frame** readFrames_;
    Ether::Frame* writeFrame_;
    int numberOfReadDesc_;
    uint8_t macAddress_[6];

    void waitInterrupt()
    {
        MessageInfo msg;
        Message::receive(&msg);

        switch (msg.header)
        {
        case MSG_INTERRUPTED:
        {
            // Read ISR and reset it.
            inp8(baseAddress_ + VIRTIO_PCI_ISR);
            monapi_set_irq(irqLine_, MONAPI_TRUE, MONAPI_TRUE);
            break;
        }
        default:
            VIRT_LOG("[virtio] uknown message");
            break;
        }
    }

    uint8_t* allocateAlignedPage()
    {
        Buffer* data = new Buffer(PAGE_SIZE * 2);
        const uintptr_t phys = syscall_get_physical_address((uintptr_t)data->data());
        const uintptr_t aphys = (phys+ PAGE_MASK) & ~PAGE_MASK;
        uint8_t* page = (uint8_t *) (data->data() + aphys - phys);
        return page;
    }

    // Data buffer for read requires 2 pages.
    // One is for header, the other for data.
    uint8_t* makeReadDescSet(struct vring* vring, int startIndex)
    {
        uint8_t* headerPage = allocateAlignedPage();
        vring->desc[startIndex].flags = VRING_DESC_F_NEXT | VRING_DESC_F_WRITE;
        vring->desc[startIndex].addr = syscall_get_physical_address((uintptr_t)headerPage);
        vring->desc[startIndex].len = sizeof(struct virtio_net_hdr);

        uint8_t* dataPage = allocateAlignedPage();
        vring->desc[startIndex + 1].flags =  VRING_DESC_F_WRITE; // no next
        vring->desc[startIndex + 1].addr = syscall_get_physical_address((uintptr_t)dataPage);
        vring->desc[startIndex + 1].len = PAGE_SIZE;
        return dataPage;
    }

    void setMacAddress()
    {
        struct virtio_net_config config;
        for (uintptr_t i = 0; i < sizeof(config); i += 4) {
            (((uint32_t*)&config)[i / 4]) = inp32(baseAddress_ + VIRTIO_PCI_CONFIG + i);
        }
        memcpy(macAddress_, config.mac, 6);
    }

    bool probeDevice()
    {
        PciInf pciInf;
        Pci pci;
        pci.CheckPciExist(PCI_VENDOR_ID_REDHAT_QUMRANET, PCI_DEVICE_ID_VIRTIO_NET, &pciInf);

        if (!pciInf.isExist) {
            return false;
        }

        VIRT_LOG("device found");

        // set up device specific data
        baseAddress_ = pciInf.baseAdress & ~1;
        irqLine_     = pciInf.irqLine;

        VIRT_LOG("baseAdress=%x irqLine=%d", baseAddress_, pciInf.irqLine);

        setMacAddress();

        // reset the device
        outp8(baseAddress_ + VIRTIO_PCI_STATUS,  VIRTIO_CONFIG_S_DRIVER | VIRTIO_CONFIG_S_DRIVER_OK); // 0: reset
        return true;
    }

    struct vring* createEmptyVring(enum VringType type)
    {
        // Select the queue to use. 0: read queue, 1: write queue
        outp16(baseAddress_ + VIRTIO_PCI_QUEUE_SEL, type);

        // How many descriptors do the queue have?
        const int numberOfDesc = inp16(baseAddress_ + VIRTIO_PCI_QUEUE_NUM);
        ASSERT(numberOfDesc == 256);

        // Check whether the queue is already set vring (necessary?).
        uint16_t pfn = inp16(baseAddress_ + VIRTIO_PCI_QUEUE_PFN);
        if (pfn != 0) {
            return NULL;
        }

        const int MAX_QUEUE_SIZE = PAGE_MASK + vring_size(MAX_QUEUE_NUM);
        Buffer* readDesc = new Buffer(MAX_QUEUE_SIZE);
        struct vring* vring = new struct vring;
        vring->num = numberOfDesc;
        // page aligned
        const uintptr_t physicalAddress = syscall_get_physical_address((uintptr_t)readDesc->data());
        const uintptr_t alignedAddress = (physicalAddress + PAGE_MASK) & ~PAGE_MASK;

        ASSERT((alignedAddress % PAGE_SIZE) == 0);

        // vring.desc is page aligned
        vring->desc = (struct vring_desc*)(readDesc->data() + alignedAddress - physicalAddress);

        // make linked ring
        for (uintptr_t i = 0; i < vring->num; i++) {
            vring->desc[i].next = i + 1;
            // page should be writable by host
            if (type == VRING_TYPE_READ) {
                vring->desc[i].flags |= VRING_DESC_F_WRITE;
            }
        }
        vring->desc[vring->num - 1].next = 0;

        // vring.avail is follow after the array of desc
        vring->avail = (struct vring_avail *)&vring->desc[numberOfDesc];

        // vring.used is also page aligned
        const uintptr_t usedPhysicalAddress = syscall_get_physical_address((uintptr_t)&(vring->avail->ring[numberOfDesc]));
        const uintptr_t usedAligendAddress = (usedPhysicalAddress + PAGE_MASK) & ~PAGE_MASK;
        ASSERT((usedAligendAddress % PAGE_SIZE) == 0);
        vring->used = (struct vring_used*)((uintptr_t)&(vring->avail->ring[numberOfDesc]) + usedAligendAddress - usedPhysicalAddress);

        ASSERT((uintptr_t)syscall_get_physical_address((uintptr_t)vring->used) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring->desc) == 8192);
        ASSERT((((uintptr_t)syscall_get_physical_address((uintptr_t)vring->used) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring->desc)) % PAGE_SIZE) == 0);
        return vring;

    }

    struct vring* createReadVring(int numberOfDescToCreate)
    {
        struct vring* vring = createEmptyVring(VRING_TYPE_READ);
        if (NULL == vring) {
            return NULL;
        }

        readFrames_ = new Ether::Frame*[numberOfDescToCreate];
        for (int i = 0; i < numberOfDescToCreate; i++) {
            readFrames_[i] = (Ether::Frame*)makeReadDescSet(vring, i * 2);
            vring->avail->ring[i] = i * 2;
        }
        vring->avail->idx = numberOfDescToCreate;

        // Wait interruption after issued VIRTIO_PCI_QUEUE_PFN.
        outp32(baseAddress_ + VIRTIO_PCI_QUEUE_PFN, syscall_get_physical_address((uintptr_t)vring->desc) >> 12);
        waitInterrupt();

        lastUsedIndexRead_ = vring->used->idx;
        numberOfReadDesc_ = numberOfDescToCreate;
        return vring;
    }

public:
    enum DeviceState {
        DEVICE_FOUND,
        DEVICE_NOT_FOUND,
        DEVICE_ALREADY_CONFIGURED,

    };

    VirtioNet()
    {
    }

    uint8_t* macAddress()
    {
        return macAddress_;
    }

    enum DeviceState probe(int numberOfReadBufferes)
    {
        // Probe virtio-net device, fetch baseAdress and irqLine.
        if (!probeDevice()) {
            VIRT_LOG("device not found");
            return DEVICE_NOT_FOUND;
        }

        // Set up for interrution
        monapi_set_irq(irqLine_, MONAPI_TRUE, MONAPI_TRUE);
        syscall_set_irq_receiver(irqLine_, SYS_MASK_INTERRUPT);

        readVring_ = createReadVring(numberOfReadBufferes);
        if (NULL == readVring_) {
            return DEVICE_ALREADY_CONFIGURED;
        }

        writeVring_ = createEmptyVring(VRING_TYPE_WRITE);
        if (NULL == writeVring_) {
            return DEVICE_ALREADY_CONFIGURED;
        }

        // this flags should be set before VIRTIO_PCI_QUEUE_PFN.
        writeVring_->avail->flags |= VRING_AVAIL_F_NO_INTERRUPT;
        outp32(baseAddress_ + VIRTIO_PCI_QUEUE_PFN, syscall_get_physical_address((uintptr_t)writeVring_->desc) >> 12);
        waitInterrupt();

        // prepare the data to write
        // virtio_net_hdr is *necessary*
        struct virtio_net_hdr* hdr = (struct virtio_net_hdr*) (allocateAlignedPage());
        hdr->flags = 0;
        hdr->csum_offset = 0;
        hdr->csum_start = 0;
        hdr->gso_type = VIRTIO_NET_HDR_GSO_NONE;
        hdr->gso_size = 0;
        hdr->hdr_len = 0;
        writeVring_->desc[0].flags  |= VRING_DESC_F_NEXT;
        writeVring_->desc[0].addr = syscall_get_physical_address((uintptr_t)hdr);
        writeVring_->desc[0].len = sizeof(struct virtio_net_hdr);

        writeFrame_ = (Ether::Frame*)(allocateAlignedPage());
        writeVring_->desc[1].flags = 0; // no next
        writeVring_->desc[1].addr = syscall_get_physical_address((uintptr_t)writeFrame_);
        writeVring_->desc[1].len = sizeof(Ether::Frame);
        lastUsedIndexWrite_ = writeVring_->used->idx;
        return DEVICE_FOUND;
    }

    // N.B.
    // Since it is hard to distiguish sent and received interruption, we don't use interruption for notification of packet send.
    // Instread before sending packet, we check whether last send is done.
    bool send(Ether::Frame* src)
    {
            logprintf("(\"%s\" %d)\n", __FILE__, __LINE__);
        while (lastUsedIndexWrite_ != writeVring_->used->idx) {
            // Wait for last send is done.
            // In almost case, we expect last send is already done.
            VIRT_LOG("Waiting previous packet is send");
        }
            logprintf("(\"%s\" %d)\n", __FILE__, __LINE__);
        memcpy(writeFrame_, src, sizeof(Ether::Frame));

        writeVring_->avail->ring[writeVring_->avail->idx % writeVring_->num] = 0; // desc[0] -> desc[1] is used for buffer
        writeVring_->avail->idx++;

        // Before notify, save the lastUsedIndexWrite_
        lastUsedIndexWrite_ = writeVring_->used->idx + 1;
            logprintf("(\"%s\" %d)\n", __FILE__, __LINE__);
        outp16(baseAddress_ + VIRTIO_PCI_QUEUE_NOTIFY, VRING_TYPE_WRITE);
            logprintf("(\"%s\" %d)\n", __FILE__, __LINE__);
        return true;
    }

    bool receive(Ether::Frame* dst)
    {
        if (readVring_->used->idx == lastUsedIndexRead_) {
            return false;
        }
            logprintf("(\"%s\" %d)\n", __FILE__, __LINE__);
//         if  (readVring_->used->idx == lastUsedIndexRead_) {
//             return false;
//         }
        waitInterrupt();
    logprintf("(\"%s\" %d)\n", __FILE__, __LINE__);
        while (readVring_->used->idx == lastUsedIndexRead_) {
            VIRT_LOG("waiting in lastUsedIndexRead_=%d", lastUsedIndexRead_);
//            _printf("[1]");791
        }
    logprintf("(\"%s\" %d)\n", __FILE__, __LINE__);
        int next_used = readVring_->used->idx;
    logprintf("(\"%s\" %d)\n", __FILE__, __LINE__);
        if (!(readVring_->used->flags & VRING_USED_F_NO_NOTIFY)) {
            VIRT_LOG("NOTIFY");
            logprintf("(\"%s\" %d)\n", __FILE__, __LINE__);
            outp16(baseAddress_ + VIRTIO_PCI_QUEUE_NOTIFY, 0);
        }
    logprintf("(\"%s\" %d)\n", __FILE__, __LINE__);
        // avail->idx % numberOfReadDesc_ is current read buffer index
        Ether::Frame* rframe = readFrames_[readVring_->avail->idx % numberOfReadDesc_];
        memcpy(dst, rframe, sizeof(Ether::Frame));
    logprintf("(\"%s\" %d)\n", __FILE__, __LINE__);

        // current used buffer is no more necessary, give it back to tail of avail->ring
        readVring_->avail->ring[(readVring_->avail->idx) % readVring_->num] = ((readVring_->avail->idx) % numberOfReadDesc_) * 2;
        logprintf("(\"%s\" %d)\n", __FILE__, __LINE__);
        // increment avail->idx, we should not take remainder of avail->idx ?
        readVring_->avail->idx++;
        lastUsedIndexRead_ = next_used;
        return true;
    }
};

void macAddressCopy(uint8_t* dest, const uint8_t* src)
{
    memcpy(dest, src, 6);
}

void makeArpReply(Ether::Frame* reply, Arp::Header* request, uint32_t myIpAddress, uint8_t* myMacAddress)
{
    macAddressCopy(reply->dstmac, request->srcMac);
    macAddressCopy(reply->srcmac, myMacAddress);
    reply->type = Util::swapShort(Ether::ARP);

    Arp::Header* header = (Arp::Header*)reply->data;
    header->hardType =Util::swapShort(Arp::HARD_TYPE_ETHER);
    header->protType = Util::swapShort(Arp::PROTCOL_TYPE_IP);
    header->hardAddrLen = 6;
    header->protAddrLen = 4;
    macAddressCopy(header->srcMac, myMacAddress);
    header->opeCode = Util::swapShort(Arp::OPE_CODE_ARP_REP);
    header->srcIp = myIpAddress;
    header->dstIp = request->srcIp;
    macAddressCopy(header->dstMac, request->srcMac);
}

unsigned int
monadev_read(VirtioNet* receiver)
{
    Ether::Frame frame;
    if (!receiver->receive(&frame)) {
        return 0;
    } else {
        memcpy(uip_buf, &frame, UIP_BUFSIZE);
        return UIP_BUFSIZE;
    }
}

void
monadev_send(VirtioNet* receiver)
{
    int ret;
    char tmpbuf[UIP_BUFSIZE];
    int i;
    for(i = 0; i < 40 + UIP_LLH_LEN; i++) {
        tmpbuf[i] = uip_buf[i];
    }

    for(; i < uip_len; i++) {
        tmpbuf[i] = uip_appdata[i - 40 - UIP_LLH_LEN];
    }

    Ether::Frame frame;
    memcpy(&frame, tmpbuf, UIP_BUFSIZE);
    receiver->send(&frame);
//     memset(&frame, 0, sizeof(frame));
//     for (int i = 0; i < 100; i++) {
//     receiver->send(&frame);
//     }

    return;
}


int main(int argc, char* argv[])
{
//    const uint32_t myIpAddress = Util::ipAddressToUint32_t(192, 168, 50, 3);

    VirtioNet receiver;
    const int numberOfReadBufferes = 5;
    enum VirtioNet::DeviceState state = receiver.probe(numberOfReadBufferes);
    if (state != VirtioNet::DEVICE_FOUND) {
        printf("[virtio] virtio-net device not found\n");
        exit(-1);
    }
    
    logprintf("(\"%s\" %d)\n", __FILE__, __LINE__);
{
    u8_t i, arptimer;

    /* Initialize the device driver. */
    /* Initialize the uIP TCP/IP stack. */
    uip_init();
    /* Initialize the HTTP server. */
    httpd_init();


    arptimer = 0;

    while(1) {
        /* Let the monadev network device driver read an entire IP packet
           into the uip_buf. If it must wait for more than 0.5 seconds, it
           will return with the return value 0. If so, we know that it is
           time to call upon the uip_periodic(). Otherwise, the monadev has
           received an IP packet that is to be processed by uIP. */
    logprintf("(\"%s\" %d)\n", __FILE__, __LINE__);
        uip_len = monadev_read(&receiver);
    logprintf("(\"%s\" %d)\n", __FILE__, __LINE__);
        if(uip_len == 0) {
    logprintf("(\"%s\" %d)\n", __FILE__, __LINE__);
            for(i = 0; i < UIP_CONNS; i++) {
                uip_periodic(i);
                /* If the above function invocation resulted in data that
                   should be sent out on the network, the global variable
                   uip_len is set to a value > 0. */
                if(uip_len > 0) {
    logprintf("(\"%s\" %d)\n", __FILE__, __LINE__);
                    uip_arp_out();
    logprintf("(\"%s\" %d)\n", __FILE__, __LINE__);
                    monadev_send(&receiver);
    logprintf("(\"%s\" %d)\n", __FILE__, __LINE__);
                }
            }

#if UIP_UDP
            for(i = 0; i < UIP_UDP_CONNS; i++) {
                uip_udp_periodic(i);
                /* If the above function invocation resulted in data that
                   should be sent out on the network, the global variable
                   uip_len is set to a value > 0. */
                if(uip_len > 0) {
                    uip_arp_out();
                    monadev_send(receiver);
                }
            }
#endif /* UIP_UDP */

            /* Call the ARP timer function every 10 seconds. */
            if(++arptimer == 20) {
    logprintf("(\"%s\" %d)\n", __FILE__, __LINE__);
                uip_arp_timer();
    logprintf("(\"%s\" %d)\n", __FILE__, __LINE__);
                arptimer = 0;
            }

        } else {
            if(BUF->type == htons(UIP_ETHTYPE_IP)) {
    logprintf("(\"%s\" %d)\n", __FILE__, __LINE__);
                uip_arp_ipin();
    logprintf("(\"%s\" %d)\n", __FILE__, __LINE__);
                uip_input();
    logprintf("(\"%s\" %d)\n", __FILE__, __LINE__);
                /* If the above function invocation resulted in data that
                   should be sent out on the network, the global variable
                   uip_len is set to a value > 0. */
                if(uip_len > 0) {
    logprintf("(\"%s\" %d)\n", __FILE__, __LINE__);
                    uip_arp_out();
                    //  logprintf("(\"%s\" %d)\n", __FILE__, __LINE__);
                    monadev_send(&receiver);
    logprintf("(\"%s\" %d)\n", __FILE__, __LINE__);
                }
            } else if(BUF->type == htons(UIP_ETHTYPE_ARP)) {
                uip_arp_arpin();
                /* If the above function invocation resulted in data that
                   should be sent out on the network, the global variable
                   uip_len is set to a value > 0. */
                if(uip_len > 0) {
    logprintf("(\"%s\" %d)\n", __FILE__, __LINE__);
                    monadev_send(&receiver);

    logprintf("(\"%s\" %d)\n", __FILE__, __LINE__);
    logprintf("uip_len = %d\n", uip_len);
                }
            }
        }

    }
    return 0;
}

//     while (true) {
//         Ether::Frame frame;
//         if (!receiver.receive(&frame)) {
//             printf("[virtio] receive failed\n");
//             exit(-1);
//         }
//         if (Util::swapShort(frame.type) == Ether::ARP) {
//             Arp::Header* arp = (Arp::Header*)frame.data;
//             if (arp->opeCode == Util::swapShort(Arp::OPE_CODE_ARP_REQ)
//                 && myIpAddress == arp->dstIp) {
//                 printf("[ARP REQ] came\n");
//                 Ether::Frame reply;
//                 makeArpReply(&reply, arp, myIpAddress, receiver.macAddress());
//                 receiver.send(&reply);
//                 uint8_t macAddress[6];
//                 memset(macAddress, 0xee, 6);
//                 makeArpReply(&reply, arp, myIpAddress, macAddress);
//                 receiver.send(&reply); // send twice test
//                 printf("[ARP REP] sent\n");
//             } else {
//                 printf("[ARP Not for me] from %d:%d:%d:%d:%d:%d\n",
//                        frame.srcmac[0],
//                        frame.srcmac[1],
//                        frame.srcmac[2],
//                        frame.srcmac[3],
//                        frame.srcmac[4],
//                        frame.srcmac[5]
//                     );
//             }
//         } else if (Util::swapShort(frame.type) == Ether::IP) {
//             IP::Header* ipHeader = (IP::Header*)frame.data;
//             const int prot = ipHeader->prot;
//             if (prot == IP::UDP) {
//                 printf("[IP/UDP]\n");
//             } else if (prot == IP::ICMP) {
//                 printf("[IP/ICMP]\n");
//             } else {
//                 printf("[IP/Other] type=%d \n", prot);
//             }
//         } else {
//             printf("Unknown packet\n");
//         }
//     }
}
