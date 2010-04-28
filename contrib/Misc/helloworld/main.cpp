#include <monapi.h>
#include <monapi/CString.h>
#include <monalibc.h>
#include <pci/Pci.h>
#include <monapi/io.h>

#include <monapi.h>

#if 1

int main(int argc, char* argv[])
{
    printf("Hello, World");
    return 0;
}


#endif

#if 0

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

class Arp
{
public:
    typedef struct
    {
        uint16_t hardType  __attribute__((packed));
        uint16_t protType  __attribute__((packed));
        uint8_t  hardAddrLen;
        uint8_t  protAddrLen;
        uint16_t opeCode  __attribute__((packed));
        uint8_t  srcMac[6];
        uint32_t   srcIp  __attribute__((packed));
        uint8_t  dstMac[6];
        uint32_t   dstIp  __attribute__((packed));
    } Header;

    enum
    {
        HARD_TYPE_ETHER= 1,
        PROTCOL_TYPE_IP=0x0800,
        /* Operation code. */
        OPE_CODE_ARP_REQ= 1,
        OPE_CODE_ARP_REP= 2,
        OPE_CODE_RARP_REQ=3,
        OPE_CODE_RARP_REP=4,
    };
};

#if 1

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

class Icmp {
public :
#pragma pack(2)
    typedef struct{
        unsigned char  type;        /* ICMPタイプ。 */
        unsigned char  code;        /* ICMPコード。 */
        unsigned short chksum;      /* チェックサム。 */
        char           data[32];     /* データ。 */
    } Header;
#pragma pack(0)
};

class UDP {
public:
 typedef struct{
    uint16_t srcport;
    uint16_t dstport;
    short len;
    uint16_t chksum;
    char   data[0];
} Header;
};

class IP {
public:
    enum {
        ICMP = 0x01,
        UDP = 0x11,
    };
    typedef struct{
        uint8_t  verhead;  /* バージョン、ヘッダ長。 */
        uint8_t  tos;      /* TOS. */
        uint16_t len;       /* トータル長。 */
        uint16_t id;        /* 識別番号。 */
        uint16_t frag;      /* フラグ、フラグメントオフセット。 */
        uint8_t  ttl;      /* Time to Live. */
        uint8_t  prot;     /* プロトコル番号。 */
        uint16_t chksum;    /* ヘッダチェックサム。 */
        uint32_t srcip;        /* 送り元IP。 */
        uint32_t dstip;        /* 宛先IP。 */
        char     data[0];
    } Header;
};


uintptr_t Buffer::startAddress = 0x9E000000;

#define CHECK_BUFFER(buf) buf.check(__FILE__, __LINE__)

#if 1

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
        while (lastUsedIndexWrite_ != writeVring_->used->idx) {
            // Wait for last send is done.
            // In almost case, we expect last send is already done.
            VIRT_LOG("Waiting previous packet is send");
        }
        memcpy(writeFrame_, src, sizeof(Ether::Frame));

        writeVring_->avail->ring[writeVring_->avail->idx % writeVring_->num] = 0; // desc[0] -> desc[1] is used for buffer
        writeVring_->avail->idx++;

        // Before notify, save the lastUsedIndexWrite_
        lastUsedIndexWrite_ = writeVring_->used->idx + 1;
        outp16(baseAddress_ + VIRTIO_PCI_QUEUE_NOTIFY, VRING_TYPE_WRITE);
        return true;
    }

    bool receive(Ether::Frame* dst)
    {
        waitInterrupt();

        while (readVring_->used->idx == lastUsedIndexRead_) {
            VIRT_LOG("waiting in lastUsedIndexRead_=%d", lastUsedIndexRead_);
        }

        int next_used = readVring_->used->idx;

        if (!(readVring_->used->flags & VRING_USED_F_NO_NOTIFY)) {
            VIRT_LOG("NOTIFY");
            outp16(baseAddress_ + VIRTIO_PCI_QUEUE_NOTIFY, 0);
        }

        // avail->idx % numberOfReadDesc_ is current read buffer index
        Ether::Frame* rframe = readFrames_[readVring_->avail->idx % numberOfReadDesc_];
        memcpy(dst, rframe, sizeof(Ether::Frame));

        // current used buffer is no more necessary, give it back to tail of avail->ring
        readVring_->avail->ring[(readVring_->avail->idx) % readVring_->num] = ((readVring_->avail->idx) % numberOfReadDesc_) * 2;

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

int main(int argc, char* argv[])
{
    uint8_t* p = (uint8_t*)0;
    *p = 0;
    return 0;
    const uint32_t myIpAddress = Util::ipAddressToUint32_t(192, 168, 50, 3);

    VirtioNet receiver;
    const int numberOfReadBufferes = 5;
    enum VirtioNet::DeviceState state = receiver.probe(numberOfReadBufferes);
    if (state != VirtioNet::DEVICE_FOUND) {
        printf("[virtio] virtio-net device not found\n");
        exit(-1);
    }

    while (true) {
        Ether::Frame frame;
        if (!receiver.receive(&frame)) {
            printf("[virtio] receive failed\n");
            exit(-1);
        }
        if (Util::swapShort(frame.type) == Ether::ARP) {
            Arp::Header* arp = (Arp::Header*)frame.data;
            if (arp->opeCode == Util::swapShort(Arp::OPE_CODE_ARP_REQ)
                && myIpAddress == arp->dstIp) {
                printf("[ARP REQ] came\n");
                Ether::Frame reply;
                makeArpReply(&reply, arp, myIpAddress, receiver.macAddress());
                receiver.send(&reply);
                uint8_t macAddress[6];
                memset(macAddress, 0xee, 6);
                makeArpReply(&reply, arp, myIpAddress, macAddress);
                receiver.send(&reply); // send twice test
                printf("[ARP REP] sent\n");
            } else {
                printf("[ARP Not for me] from %d:%d:%d:%d:%d:%d\n",
                       frame.srcmac[0],
                       frame.srcmac[1],
                       frame.srcmac[2],
                       frame.srcmac[3],
                       frame.srcmac[4],
                       frame.srcmac[5]
                    );
            }
        } else if (Util::swapShort(frame.type) == Ether::IP) {
            IP::Header* ipHeader = (IP::Header*)frame.data;
            const int prot = ipHeader->prot;
            if (prot == IP::UDP) {
                printf("[IP/UDP]\n");
            } else if (prot == IP::ICMP) {
                printf("[IP/ICMP]\n");
            } else {
                printf("[IP/Other] type=%d \n", prot);
            }
        } else {
            printf("Unknown packet\n");
        }
    }
}
#endif

#if 0
// [hdr1] [data1], [hdr2] [data2],[hdr3] [data3] を動かす。使ったバッファはすぐに返却されるパターン
// + 割り込みが毎回来るのか？確認。
// →あり。うごいた。毎回来る。割り込みが来たことをきっかけとして動いて良い
int main(int argc, char* argv[])
{
    // 1. Device probe
    PciInf pciInf;
    Pci pci;
    pci.CheckPciExist(PCI_VENDOR_ID_REDHAT_QUMRANET, PCI_DEVICE_ID_VIRTIO_NET, &pciInf);

    if (!pciInf.isExist) {
        printf("device not found\n");
        exit(-1);
    }

#if 0
    printf("device found\n");
    printf("baseAdress=%x\n", pciInf.baseAdress);
    printf("irqLine=%x\n", pciInf.irqLine);
#endif

    const uintptr_t baseAddress = pciInf.baseAdress & ~1;

        struct virtio_net_config config;
        for (uintptr_t i = 0; i < sizeof(config); i += 4) {
            (((uint32_t*)&config)[i / 4]) = inp32(baseAddress + VIRTIO_PCI_CONFIG + i);
        }

#if 1
        printf("config.mac= %x:%x:%x:%x:%x:%x\n", config.mac[0], config.mac[1], config.mac[2], config.mac[3], config.mac[4], config.mac[5]);
#endif

    // 2. reset the device
    outp8(baseAddress + VIRTIO_PCI_STATUS,  VIRTIO_CONFIG_S_DRIVER | VIRTIO_CONFIG_S_DRIVER_OK); // 0: reset
#if 0
    printf("[virtio] isr=%x\n", inp8(baseAddress + VIRTIO_PCI_ISR)); // clear ISR.
#endif

    // 3. IRQ receiver
    monapi_set_irq(pciInf.irqLine, MONAPI_TRUE, MONAPI_TRUE);
    syscall_set_irq_receiver(pciInf.irqLine, SYS_MASK_INTERRUPT);

// here
    // setup receive ring
    // 4. Select the queue to use
    outp16(baseAddress + VIRTIO_PCI_QUEUE_SEL, 0); // 0: read queue, 1: write queue
    // 5. how many descriptors do the queue have?
    const int numberOfDesc2 = inp16(baseAddress + VIRTIO_PCI_QUEUE_NUM);

    printf("[ARP Request] Who has 192.168.50.2? Tell 192.168.50.3\n");

#if 0

    printf("[virtio] numberOfDesc2=%d\n", numberOfDesc2);
#endif
    ASSERT(numberOfDesc2 == 256);

    // 6. Check wheter the queue is already set vring (necessary?).
    uint16_t pfn2 = inp16(baseAddress + VIRTIO_PCI_QUEUE_PFN);

    if (pfn2 != 0) {
        printf("[virtio] pfn2=%x\n", pfn2);
        exit(-1);
    }

    // 7. setup vring
    const int MAX_QUEUE_SIZE = PAGE_MASK + vring_size(MAX_QUEUE_NUM);
    Buffer readDesc(MAX_QUEUE_SIZE);
    struct vring vring2;
    vring2.num = numberOfDesc2;
    // page align is required
    const uintptr_t physicalAddress2 = syscall_get_physical_address((uintptr_t)readDesc.data(), NULL);
    const uintptr_t alignedAddress2 = (physicalAddress2 + PAGE_MASK) & ~PAGE_MASK;

    ASSERT((alignedAddress2 % PAGE_SIZE) == 0);

    // vring.desc is page aligned
    vring2.desc = (struct vring_desc*)(readDesc.data() + alignedAddress2 - physicalAddress2);

    // make linked ring
    for (uintptr_t i = 0; i < vring2.num; i++) {
        vring2.desc[i].next = i + 1;
        vring2.desc[i].flags |= VRING_DESC_F_WRITE;
    }
    vring2.desc[vring2.num - 1].next = 0;


    // vring.avail is follow after the array of desc
    vring2.avail = (struct vring_avail *)&vring2.desc[numberOfDesc2];

    // vring.used is also page aligned
    const uintptr_t usedPhysicalAddress2 = syscall_get_physical_address((uintptr_t)&(vring2.avail->ring[numberOfDesc2]), NULL);
    const uintptr_t usedAligendAddress2 = (usedPhysicalAddress2 + PAGE_MASK) & ~PAGE_MASK;
    ASSERT((usedAligendAddress2 % PAGE_SIZE) == 0);
    vring2.used = (struct vring_used*)((uintptr_t)&(vring2.avail->ring[numberOfDesc2]) + usedAligendAddress2 - usedPhysicalAddress2);

    ASSERT((uintptr_t)syscall_get_physical_address((uintptr_t)vring2.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring2.desc, NULL) == 8192);
    ASSERT((((uintptr_t)syscall_get_physical_address((uintptr_t)vring2.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring2.desc, NULL)) % PAGE_SIZE) == 0);

    // 9. set up pfn
    outp32(baseAddress + VIRTIO_PCI_QUEUE_PFN, syscall_get_physical_address((uintptr_t)vring2.desc, NULL) >> 12);

    // 10. prepare the data to write
    Buffer readData1(PAGE_SIZE * 2);
    const uintptr_t phys3 = syscall_get_physical_address((uintptr_t)readData1.data(), NULL);
    const uintptr_t aphys3 = (phys3+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata = (uint8_t *) (readData1.data() + aphys3 - phys3);

    vring2.desc[0].flags = VRING_DESC_F_NEXT | VRING_DESC_F_WRITE; // no next
    vring2.desc[0].addr = syscall_get_physical_address((uintptr_t)rdata, NULL);
    vring2.desc[0].len = sizeof(struct virtio_net_hdr);

    Buffer readData2(PAGE_SIZE * 2);
    const uintptr_t phys4 = syscall_get_physical_address((uintptr_t)readData2.data(), NULL);
    const uintptr_t aphys4 = (phys4+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata2 = (uint8_t *) (readData2.data() + aphys4 - phys4);

    vring2.desc[1].flags = VRING_DESC_F_NEXT | VRING_DESC_F_WRITE; // no next
    vring2.desc[1].addr = syscall_get_physical_address((uintptr_t)rdata2, NULL);
    vring2.desc[1].len = PAGE_SIZE;

    Buffer readData3(PAGE_SIZE * 2);
    const uintptr_t phys5 = syscall_get_physical_address((uintptr_t)readData3.data(), NULL);
    const uintptr_t aphys5 = (phys5+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata3 = (uint8_t *) (readData3.data() + aphys5 - phys5);

    vring2.desc[2].flags = VRING_DESC_F_NEXT | VRING_DESC_F_WRITE; // no next
    vring2.desc[2].addr = syscall_get_physical_address((uintptr_t)rdata3, NULL);
    vring2.desc[2].len = sizeof(struct virtio_net_hdr);

    Buffer readData4(PAGE_SIZE * 2);
    const uintptr_t phys6 = syscall_get_physical_address((uintptr_t)readData4.data(), NULL);
    const uintptr_t aphys6 = (phys6+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata4 = (uint8_t *) (readData4.data() + aphys6 - phys6);

    vring2.desc[3].flags = VRING_DESC_F_NEXT | VRING_DESC_F_WRITE; // no next
    vring2.desc[3].addr = syscall_get_physical_address((uintptr_t)rdata4, NULL);
    vring2.desc[3].len = PAGE_SIZE;

    Buffer readData5(PAGE_SIZE * 2);
    const uintptr_t phys7 = syscall_get_physical_address((uintptr_t)readData5.data(), NULL);
    const uintptr_t aphys7 = (phys7+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata5 = (uint8_t *) (readData3.data() + aphys7 - phys7);

    vring2.desc[4].flags = VRING_DESC_F_NEXT | VRING_DESC_F_WRITE; // no next
    vring2.desc[4].addr = syscall_get_physical_address((uintptr_t)rdata5, NULL);
    vring2.desc[4].len = sizeof(struct virtio_net_hdr);

    Buffer readData6(PAGE_SIZE * 2);
    const uintptr_t phys8 = syscall_get_physical_address((uintptr_t)readData6.data(), NULL);
    const uintptr_t aphys8 = (phys8+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata6 = (uint8_t *) (readData6.data() + aphys8 - phys8);

    vring2.desc[5].flags = VRING_DESC_F_NEXT | VRING_DESC_F_WRITE; // no next
    vring2.desc[5].addr = syscall_get_physical_address((uintptr_t)rdata6, NULL);
    vring2.desc[5].len = PAGE_SIZE;


    vring2.avail->ring[0] =0;
    vring2.avail->ring[1] =2;
    vring2.avail->ring[2] =4;

    vring2.avail->idx = 3;



    int last_used_idx = vring2.used->idx;

    outp16(baseAddress + VIRTIO_PCI_QUEUE_NOTIFY, 0);

    Ether::Frame* rframes[3];
    rframes[0] = (Ether::Frame*)rdata2;
    rframes[1] = (Ether::Frame*)rdata4;
    rframes[2] = (Ether::Frame*)rdata6;
   while (true) {

    MessageInfo msg;
    Message::receive(&msg);

    printf("waiting interrupt \n");

    switch (msg.header)
    {
    case MSG_INTERRUPTED:
    {
        // clear ISR.
        const uint8_t isr = inp8(baseAddress + VIRTIO_PCI_ISR);
        monapi_set_irq(pciInf.irqLine, MONAPI_TRUE, MONAPI_TRUE);
        break;
    }
    default:
        printf("[virtio] uknown message\n");
        exit(-1);
        break;
    }
       printf("waiting \n");

       while (vring2.used->idx == last_used_idx) {

       printf("waiting in\n");
//           sleep(20);
       }



       int next_used = vring2.used->idx;
//       ASSERT(1 == next_used - last_used_idx);

       printf("used_idx %d => %d\n", last_used_idx, next_used);

       if (!(vring2.used->flags & VRING_USED_F_NO_NOTIFY)) {
           printf("*** NOTIFY ****\n");
           outp16(baseAddress + VIRTIO_PCI_QUEUE_NOTIFY, 0);
       }

       // どのバッファかは % で分かる。
       Ether::Frame* rframe = rframes[vring2.avail->idx % 3];
    printf("[Ether1] %x from %d:%d:%d:%d:%d:%d\n",
           Util::swapShort(rframe->type),
           rframe->srcmac[0],
           rframe->srcmac[1],
           rframe->srcmac[2],
           rframe->srcmac[3],
           rframe->srcmac[4],
           rframe->srcmac[5]
           );

    // 使用済みのバッファは直ちに末尾にセットする
    vring2.avail->ring[(vring2.avail->idx) % vring2.num] = ((vring2.avail->idx) % 3) * 2;
    vring2.avail->idx = vring2.avail->idx + 1; // ここは剰余をとってはいけない。255を超えてもホスト側で面倒を見てくれてる？


    last_used_idx = next_used;

   }
    }
#endif
#if 0
// [hdr1] [data1], [hdr2] [data2],[hdr3] [data3] を動かす。使ったバッファはすぐに返却されるパターン
// →あり。うごいた。
int main(int argc, char* argv[])
{
    // 1. Device probe
    PciInf pciInf;
    Pci pci;
    pci.CheckPciExist(PCI_VENDOR_ID_REDHAT_QUMRANET, PCI_DEVICE_ID_VIRTIO_NET, &pciInf);

    if (!pciInf.isExist) {
        printf("device not found\n");
        exit(-1);
    }

#if 0
    printf("device found\n");
    printf("baseAdress=%x\n", pciInf.baseAdress);
    printf("irqLine=%x\n", pciInf.irqLine);
#endif

    const uintptr_t baseAddress = pciInf.baseAdress & ~1;

        struct virtio_net_config config;
        for (uintptr_t i = 0; i < sizeof(config); i += 4) {
            (((uint32_t*)&config)[i / 4]) = inp32(baseAddress + VIRTIO_PCI_CONFIG + i);
        }

#if 1
        printf("config.mac= %x:%x:%x:%x:%x:%x\n", config.mac[0], config.mac[1], config.mac[2], config.mac[3], config.mac[4], config.mac[5]);
#endif

    // 2. reset the device
    outp8(baseAddress + VIRTIO_PCI_STATUS,  VIRTIO_CONFIG_S_DRIVER | VIRTIO_CONFIG_S_DRIVER_OK); // 0: reset
#if 0
    printf("[virtio] isr=%x\n", inp8(baseAddress + VIRTIO_PCI_ISR)); // clear ISR.
#endif

    // 3. IRQ receiver
    monapi_set_irq(pciInf.irqLine, MONAPI_TRUE, MONAPI_TRUE);
    syscall_set_irq_receiver(pciInf.irqLine, SYS_MASK_INTERRUPT);

// here
    // setup receive ring
    // 4. Select the queue to use
    outp16(baseAddress + VIRTIO_PCI_QUEUE_SEL, 0); // 0: read queue, 1: write queue
    // 5. how many descriptors do the queue have?
    const int numberOfDesc2 = inp16(baseAddress + VIRTIO_PCI_QUEUE_NUM);

    printf("[ARP Request] Who has 192.168.50.2? Tell 192.168.50.3\n");

#if 0

    printf("[virtio] numberOfDesc2=%d\n", numberOfDesc2);
#endif
    ASSERT(numberOfDesc2 == 256);

    // 6. Check wheter the queue is already set vring (necessary?).
    uint16_t pfn2 = inp16(baseAddress + VIRTIO_PCI_QUEUE_PFN);

    if (pfn2 != 0) {
        printf("[virtio] pfn2=%x\n", pfn2);
        exit(-1);
    }

    // 7. setup vring
    const int MAX_QUEUE_SIZE = PAGE_MASK + vring_size(MAX_QUEUE_NUM);
    Buffer readDesc(MAX_QUEUE_SIZE);
    struct vring vring2;
    vring2.num = numberOfDesc2;
    // page align is required
    const uintptr_t physicalAddress2 = syscall_get_physical_address((uintptr_t)readDesc.data(), NULL);
    const uintptr_t alignedAddress2 = (physicalAddress2 + PAGE_MASK) & ~PAGE_MASK;

    ASSERT((alignedAddress2 % PAGE_SIZE) == 0);

    // vring.desc is page aligned
    vring2.desc = (struct vring_desc*)(readDesc.data() + alignedAddress2 - physicalAddress2);

    // make linked ring
    for (uintptr_t i = 0; i < vring2.num; i++) {
        vring2.desc[i].next = i + 1;
        vring2.desc[i].flags |= VRING_DESC_F_WRITE;
    }
    vring2.desc[vring2.num - 1].next = 0;


    // vring.avail is follow after the array of desc
    vring2.avail = (struct vring_avail *)&vring2.desc[numberOfDesc2];

    // vring.used is also page aligned
    const uintptr_t usedPhysicalAddress2 = syscall_get_physical_address((uintptr_t)&(vring2.avail->ring[numberOfDesc2]), NULL);
    const uintptr_t usedAligendAddress2 = (usedPhysicalAddress2 + PAGE_MASK) & ~PAGE_MASK;
    ASSERT((usedAligendAddress2 % PAGE_SIZE) == 0);
    vring2.used = (struct vring_used*)((uintptr_t)&(vring2.avail->ring[numberOfDesc2]) + usedAligendAddress2 - usedPhysicalAddress2);

    ASSERT((uintptr_t)syscall_get_physical_address((uintptr_t)vring2.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring2.desc, NULL) == 8192);
    ASSERT((((uintptr_t)syscall_get_physical_address((uintptr_t)vring2.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring2.desc, NULL)) % PAGE_SIZE) == 0);

    // 9. set up pfn
    outp32(baseAddress + VIRTIO_PCI_QUEUE_PFN, syscall_get_physical_address((uintptr_t)vring2.desc, NULL) >> 12);

    // 10. prepare the data to write
    Buffer readData1(PAGE_SIZE * 2);
    const uintptr_t phys3 = syscall_get_physical_address((uintptr_t)readData1.data(), NULL);
    const uintptr_t aphys3 = (phys3+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata = (uint8_t *) (readData1.data() + aphys3 - phys3);

    vring2.desc[0].flags = VRING_DESC_F_NEXT | VRING_DESC_F_WRITE; // no next
    vring2.desc[0].addr = syscall_get_physical_address((uintptr_t)rdata, NULL);
    vring2.desc[0].len = sizeof(struct virtio_net_hdr);

    Buffer readData2(PAGE_SIZE * 2);
    const uintptr_t phys4 = syscall_get_physical_address((uintptr_t)readData2.data(), NULL);
    const uintptr_t aphys4 = (phys4+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata2 = (uint8_t *) (readData2.data() + aphys4 - phys4);

    vring2.desc[1].flags = VRING_DESC_F_NEXT | VRING_DESC_F_WRITE; // no next
    vring2.desc[1].addr = syscall_get_physical_address((uintptr_t)rdata2, NULL);
    vring2.desc[1].len = PAGE_SIZE;

    Buffer readData3(PAGE_SIZE * 2);
    const uintptr_t phys5 = syscall_get_physical_address((uintptr_t)readData3.data(), NULL);
    const uintptr_t aphys5 = (phys5+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata3 = (uint8_t *) (readData3.data() + aphys5 - phys5);

    vring2.desc[2].flags = VRING_DESC_F_NEXT | VRING_DESC_F_WRITE; // no next
    vring2.desc[2].addr = syscall_get_physical_address((uintptr_t)rdata3, NULL);
    vring2.desc[2].len = sizeof(struct virtio_net_hdr);

    Buffer readData4(PAGE_SIZE * 2);
    const uintptr_t phys6 = syscall_get_physical_address((uintptr_t)readData4.data(), NULL);
    const uintptr_t aphys6 = (phys6+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata4 = (uint8_t *) (readData4.data() + aphys6 - phys6);

    vring2.desc[3].flags = VRING_DESC_F_NEXT | VRING_DESC_F_WRITE; // no next
    vring2.desc[3].addr = syscall_get_physical_address((uintptr_t)rdata4, NULL);
    vring2.desc[3].len = PAGE_SIZE;

    Buffer readData5(PAGE_SIZE * 2);
    const uintptr_t phys7 = syscall_get_physical_address((uintptr_t)readData5.data(), NULL);
    const uintptr_t aphys7 = (phys7+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata5 = (uint8_t *) (readData3.data() + aphys7 - phys7);

    vring2.desc[4].flags = VRING_DESC_F_NEXT | VRING_DESC_F_WRITE; // no next
    vring2.desc[4].addr = syscall_get_physical_address((uintptr_t)rdata5, NULL);
    vring2.desc[4].len = sizeof(struct virtio_net_hdr);

    Buffer readData6(PAGE_SIZE * 2);
    const uintptr_t phys8 = syscall_get_physical_address((uintptr_t)readData6.data(), NULL);
    const uintptr_t aphys8 = (phys8+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata6 = (uint8_t *) (readData6.data() + aphys8 - phys8);

    vring2.desc[5].flags = VRING_DESC_F_NEXT | VRING_DESC_F_WRITE; // no next
    vring2.desc[5].addr = syscall_get_physical_address((uintptr_t)rdata6, NULL);
    vring2.desc[5].len = PAGE_SIZE;


    vring2.avail->ring[0] =0;
    vring2.avail->ring[1] =2;
    vring2.avail->ring[2] =4;

    vring2.avail->idx = 3;



    int last_used_idx = vring2.used->idx;

    outp16(baseAddress + VIRTIO_PCI_QUEUE_NOTIFY, 0);

    Ether::Frame* rframes[3];
    rframes[0] = (Ether::Frame*)rdata2;
    rframes[1] = (Ether::Frame*)rdata4;
    rframes[2] = (Ether::Frame*)rdata6;
   while (true) {
       printf("waiting \n");
       while (vring2.used->idx == last_used_idx) {
//           sleep(20);
       }

       int next_used = vring2.used->idx;
       ASSERT(1 == next_used - last_used_idx);

       printf("used_idx %d => %d\n", last_used_idx, next_used);

       if (!(vring2.used->flags & VRING_USED_F_NO_NOTIFY)) {
           printf("*** NOTIFY ****\n");
           outp16(baseAddress + VIRTIO_PCI_QUEUE_NOTIFY, 0);
       }

       // どのバッファかは % で分かる。
       Ether::Frame* rframe = rframes[vring2.avail->idx % 3];
    printf("[Ether1] %x from %d:%d:%d:%d:%d:%d\n",
           Util::swapShort(rframe->type),
           rframe->srcmac[0],
           rframe->srcmac[1],
           rframe->srcmac[2],
           rframe->srcmac[3],
           rframe->srcmac[4],
           rframe->srcmac[5]
           );

    // 使用済みのバッファは直ちに末尾にセットする
    vring2.avail->ring[(vring2.avail->idx) % vring2.num] = ((vring2.avail->idx) % 3) * 2;
    vring2.avail->idx = (vring2.avail->idx + 1) % vring2.num;


    last_used_idx = next_used;

   }
    }

#endif

#if 0
// [hdr1] [data1], [hdr2] [data2] というのはありか？
// →あり。うごいた。
int main(int argc, char* argv[])
{
    // 1. Device probe
    PciInf pciInf;
    Pci pci;
    pci.CheckPciExist(PCI_VENDOR_ID_REDHAT_QUMRANET, PCI_DEVICE_ID_VIRTIO_NET, &pciInf);

    if (!pciInf.isExist) {
        printf("device not found\n");
        exit(-1);
    }

#if 0
    printf("device found\n");
    printf("baseAdress=%x\n", pciInf.baseAdress);
    printf("irqLine=%x\n", pciInf.irqLine);
#endif

    const uintptr_t baseAddress = pciInf.baseAdress & ~1;

        struct virtio_net_config config;
        for (uintptr_t i = 0; i < sizeof(config); i += 4) {
            (((uint32_t*)&config)[i / 4]) = inp32(baseAddress + VIRTIO_PCI_CONFIG + i);
        }

#if 1
        printf("config.mac= %x:%x:%x:%x:%x:%x\n", config.mac[0], config.mac[1], config.mac[2], config.mac[3], config.mac[4], config.mac[5]);
#endif

    // 2. reset the device
    outp8(baseAddress + VIRTIO_PCI_STATUS,  VIRTIO_CONFIG_S_DRIVER | VIRTIO_CONFIG_S_DRIVER_OK); // 0: reset
#if 0
    printf("[virtio] isr=%x\n", inp8(baseAddress + VIRTIO_PCI_ISR)); // clear ISR.
#endif

    // 3. IRQ receiver
    monapi_set_irq(pciInf.irqLine, MONAPI_TRUE, MONAPI_TRUE);
    syscall_set_irq_receiver(pciInf.irqLine, SYS_MASK_INTERRUPT);

// here
    // setup receive ring
    // 4. Select the queue to use
    outp16(baseAddress + VIRTIO_PCI_QUEUE_SEL, 0); // 0: read queue, 1: write queue
    // 5. how many descriptors do the queue have?
    const int numberOfDesc2 = inp16(baseAddress + VIRTIO_PCI_QUEUE_NUM);

    printf("[ARP Request] Who has 192.168.50.2? Tell 192.168.50.3\n");

#if 0

    printf("[virtio] numberOfDesc2=%d\n", numberOfDesc2);
#endif
    ASSERT(numberOfDesc2 == 256);

    // 6. Check wheter the queue is already set vring (necessary?).
    uint16_t pfn2 = inp16(baseAddress + VIRTIO_PCI_QUEUE_PFN);

    if (pfn2 != 0) {
        printf("[virtio] pfn2=%x\n", pfn2);
        exit(-1);
    }

    // 7. setup vring
    const int MAX_QUEUE_SIZE = PAGE_MASK + vring_size(MAX_QUEUE_NUM);
    Buffer readDesc(MAX_QUEUE_SIZE);
    struct vring vring2;
    vring2.num = numberOfDesc2;
    // page align is required
    const uintptr_t physicalAddress2 = syscall_get_physical_address((uintptr_t)readDesc.data(), NULL);
    const uintptr_t alignedAddress2 = (physicalAddress2 + PAGE_MASK) & ~PAGE_MASK;

    ASSERT((alignedAddress2 % PAGE_SIZE) == 0);

    // vring.desc is page aligned
    vring2.desc = (struct vring_desc*)(readDesc.data() + alignedAddress2 - physicalAddress2);

    // make linked ring
    for (uintptr_t i = 0; i < vring2.num; i++) {
        vring2.desc[i].next = i + 1;
        vring2.desc[i].flags |= VRING_DESC_F_WRITE;
    }
    vring2.desc[vring2.num - 1].next = 0;


    // vring.avail is follow after the array of desc
    vring2.avail = (struct vring_avail *)&vring2.desc[numberOfDesc2];

    // vring.used is also page aligned
    const uintptr_t usedPhysicalAddress2 = syscall_get_physical_address((uintptr_t)&(vring2.avail->ring[numberOfDesc2]), NULL);
    const uintptr_t usedAligendAddress2 = (usedPhysicalAddress2 + PAGE_MASK) & ~PAGE_MASK;
    ASSERT((usedAligendAddress2 % PAGE_SIZE) == 0);
    vring2.used = (struct vring_used*)((uintptr_t)&(vring2.avail->ring[numberOfDesc2]) + usedAligendAddress2 - usedPhysicalAddress2);

    ASSERT((uintptr_t)syscall_get_physical_address((uintptr_t)vring2.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring2.desc, NULL) == 8192);
    ASSERT((((uintptr_t)syscall_get_physical_address((uintptr_t)vring2.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring2.desc, NULL)) % PAGE_SIZE) == 0);

    // 9. set up pfn
    outp32(baseAddress + VIRTIO_PCI_QUEUE_PFN, syscall_get_physical_address((uintptr_t)vring2.desc, NULL) >> 12);

    // 10. prepare the data to write
    Buffer readData1(PAGE_SIZE * 2);
    const uintptr_t phys3 = syscall_get_physical_address((uintptr_t)readData1.data(), NULL);
    const uintptr_t aphys3 = (phys3+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata = (uint8_t *) (readData1.data() + aphys3 - phys3);

    vring2.desc[0].flags = VRING_DESC_F_NEXT | VRING_DESC_F_WRITE; // no next
    vring2.desc[0].addr = syscall_get_physical_address((uintptr_t)rdata, NULL);
    vring2.desc[0].len = sizeof(struct virtio_net_hdr);

    Buffer readData2(PAGE_SIZE * 2);
    const uintptr_t phys4 = syscall_get_physical_address((uintptr_t)readData2.data(), NULL);
    const uintptr_t aphys4 = (phys4+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata2 = (uint8_t *) (readData2.data() + aphys4 - phys4);

    vring2.desc[1].flags = VRING_DESC_F_NEXT | VRING_DESC_F_WRITE; // no next
    vring2.desc[1].addr = syscall_get_physical_address((uintptr_t)rdata2, NULL);
    vring2.desc[1].len = PAGE_SIZE;

    Buffer readData3(PAGE_SIZE * 2);
    const uintptr_t phys5 = syscall_get_physical_address((uintptr_t)readData3.data(), NULL);
    const uintptr_t aphys5 = (phys5+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata3 = (uint8_t *) (readData3.data() + aphys5 - phys5);

    vring2.desc[2].flags = VRING_DESC_F_NEXT | VRING_DESC_F_WRITE; // no next
    vring2.desc[2].addr = syscall_get_physical_address((uintptr_t)rdata3, NULL);
    vring2.desc[2].len = sizeof(struct virtio_net_hdr);

    Buffer readData4(PAGE_SIZE * 2);
    const uintptr_t phys6 = syscall_get_physical_address((uintptr_t)readData4.data(), NULL);
    const uintptr_t aphys6 = (phys6+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata4 = (uint8_t *) (readData4.data() + aphys5 - phys6);

    vring2.desc[3].flags = VRING_DESC_F_NEXT | VRING_DESC_F_WRITE; // no next
    vring2.desc[3].addr = syscall_get_physical_address((uintptr_t)rdata4, NULL);
    vring2.desc[3].len = PAGE_SIZE;

    vring2.avail->ring[0] =0;
    vring2.avail->ring[1] =2;

    vring2.avail->idx = 2;



    int last_used_idx = vring2.used->idx;

    outp16(baseAddress + VIRTIO_PCI_QUEUE_NOTIFY, 0);


   while (true) {
       printf("waiting \n");
       while (vring2.used->idx == last_used_idx) {
//           sleep(20);
       }

       int next_used = vring2.used->idx;
       ASSERT(1 == next_used - last_used_idx);

       printf("used_idx %d => %d\n", last_used_idx, next_used);

       if (!(vring2.used->flags & VRING_USED_F_NO_NOTIFY)) {
           printf("*** NOTIFY ****\n");
           outp16(baseAddress + VIRTIO_PCI_QUEUE_NOTIFY, 0);
       }

    Ether::Frame* rframe = (Ether::Frame*)rdata2;
    printf("[Ether1] %x from %d:%d:%d:%d:%d:%d\n",
           Util::swapShort(rframe->type),
           rframe->srcmac[0],
           rframe->srcmac[1],
           rframe->srcmac[2],
           rframe->srcmac[3],
           rframe->srcmac[4],
           rframe->srcmac[5]
           );

    last_used_idx = next_used;

    printf("waiting2 \n");

    memset(rframe, 0, sizeof(Ether::Frame));
       while (vring2.used->idx == last_used_idx) {
//           sleep(20);
       }
       next_used = vring2.used->idx;
       printf("used_idx %d => %d\n", last_used_idx, next_used);


    Ether::Frame* rframe2 = (Ether::Frame*)rdata4;
    printf("[Ether2] %x from %d:%d:%d:%d:%d:%d\n",
           Util::swapShort(rframe2->type),
           rframe2->srcmac[0],
           rframe2->srcmac[1],
           rframe2->srcmac[2],
           rframe2->srcmac[3],
           rframe2->srcmac[4],
           rframe2->srcmac[5]
           );

    last_used_idx = next_used;


    vring2.avail->ring[(vring2.avail->idx) % vring2.num] = 0;
    vring2.avail->ring[(vring2.avail->idx + 1) % vring2.num] = 2;
    vring2.avail->idx = (vring2.avail->idx + 2) % vring2.num;


    memset(rframe2, 0, sizeof(Ether::Frame));

   }
    }
#endif

#if 0
// [hdr] [data1] [data2] というのはありか？
// →なし。
int main(int argc, char* argv[])
{
    // 1. Device probe
    PciInf pciInf;
    Pci pci;
    pci.CheckPciExist(PCI_VENDOR_ID_REDHAT_QUMRANET, PCI_DEVICE_ID_VIRTIO_NET, &pciInf);

    if (!pciInf.isExist) {
        printf("device not found\n");
        exit(-1);
    }

#if 0
    printf("device found\n");
    printf("baseAdress=%x\n", pciInf.baseAdress);
    printf("irqLine=%x\n", pciInf.irqLine);
#endif

    const uintptr_t baseAddress = pciInf.baseAdress & ~1;

        struct virtio_net_config config;
        for (uintptr_t i = 0; i < sizeof(config); i += 4) {
            (((uint32_t*)&config)[i / 4]) = inp32(baseAddress + VIRTIO_PCI_CONFIG + i);
        }

#if 1
        printf("config.mac= %x:%x:%x:%x:%x:%x\n", config.mac[0], config.mac[1], config.mac[2], config.mac[3], config.mac[4], config.mac[5]);
#endif

    // 2. reset the device
    outp8(baseAddress + VIRTIO_PCI_STATUS,  VIRTIO_CONFIG_S_DRIVER | VIRTIO_CONFIG_S_DRIVER_OK); // 0: reset
#if 0
    printf("[virtio] isr=%x\n", inp8(baseAddress + VIRTIO_PCI_ISR)); // clear ISR.
#endif

    // 3. IRQ receiver
    monapi_set_irq(pciInf.irqLine, MONAPI_TRUE, MONAPI_TRUE);
    syscall_set_irq_receiver(pciInf.irqLine, SYS_MASK_INTERRUPT);

// here
    // setup receive ring
    // 4. Select the queue to use
    outp16(baseAddress + VIRTIO_PCI_QUEUE_SEL, 0); // 0: read queue, 1: write queue
    // 5. how many descriptors do the queue have?
    const int numberOfDesc2 = inp16(baseAddress + VIRTIO_PCI_QUEUE_NUM);

    printf("[ARP Request] Who has 192.168.50.2? Tell 192.168.50.3\n");

#if 0

    printf("[virtio] numberOfDesc2=%d\n", numberOfDesc2);
#endif
    ASSERT(numberOfDesc2 == 256);

    // 6. Check wheter the queue is already set vring (necessary?).
    uint16_t pfn2 = inp16(baseAddress + VIRTIO_PCI_QUEUE_PFN);

    if (pfn2 != 0) {
        printf("[virtio] pfn2=%x\n", pfn2);
        exit(-1);
    }

    // 7. setup vring
    const int MAX_QUEUE_SIZE = PAGE_MASK + vring_size(MAX_QUEUE_NUM);
    Buffer readDesc(MAX_QUEUE_SIZE);
    struct vring vring2;
    vring2.num = numberOfDesc2;
    // page align is required
    const uintptr_t physicalAddress2 = syscall_get_physical_address((uintptr_t)readDesc.data(), NULL);
    const uintptr_t alignedAddress2 = (physicalAddress2 + PAGE_MASK) & ~PAGE_MASK;

    ASSERT((alignedAddress2 % PAGE_SIZE) == 0);

    // vring.desc is page aligned
    vring2.desc = (struct vring_desc*)(readDesc.data() + alignedAddress2 - physicalAddress2);

    // make linked ring
    for (uintptr_t i = 0; i < vring2.num; i++) {
        vring2.desc[i].next = i + 1;
        vring2.desc[i].flags |= VRING_DESC_F_WRITE;
    }
    vring2.desc[vring2.num - 1].next = 0;


    // vring.avail is follow after the array of desc
    vring2.avail = (struct vring_avail *)&vring2.desc[numberOfDesc2];

    // vring.used is also page aligned
    const uintptr_t usedPhysicalAddress2 = syscall_get_physical_address((uintptr_t)&(vring2.avail->ring[numberOfDesc2]), NULL);
    const uintptr_t usedAligendAddress2 = (usedPhysicalAddress2 + PAGE_MASK) & ~PAGE_MASK;
    ASSERT((usedAligendAddress2 % PAGE_SIZE) == 0);
    vring2.used = (struct vring_used*)((uintptr_t)&(vring2.avail->ring[numberOfDesc2]) + usedAligendAddress2 - usedPhysicalAddress2);

    ASSERT((uintptr_t)syscall_get_physical_address((uintptr_t)vring2.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring2.desc, NULL) == 8192);
    ASSERT((((uintptr_t)syscall_get_physical_address((uintptr_t)vring2.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring2.desc, NULL)) % PAGE_SIZE) == 0);

    // 9. set up pfn
    outp32(baseAddress + VIRTIO_PCI_QUEUE_PFN, syscall_get_physical_address((uintptr_t)vring2.desc, NULL) >> 12);

    // 10. prepare the data to write
    Buffer readData1(PAGE_SIZE * 2);
    const uintptr_t phys3 = syscall_get_physical_address((uintptr_t)readData1.data(), NULL);
    const uintptr_t aphys3 = (phys3+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata = (uint8_t *) (readData1.data() + aphys3 - phys3);

    vring2.desc[0].flags = VRING_DESC_F_NEXT | VRING_DESC_F_WRITE; // no next
    vring2.desc[0].addr = syscall_get_physical_address((uintptr_t)rdata, NULL);
    vring2.desc[0].len = sizeof(struct virtio_net_hdr);

    Buffer readData2(PAGE_SIZE * 2);
    const uintptr_t phys4 = syscall_get_physical_address((uintptr_t)readData2.data(), NULL);
    const uintptr_t aphys4 = (phys4+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata2 = (uint8_t *) (readData2.data() + aphys4 - phys4);

    vring2.desc[1].flags = VRING_DESC_F_NEXT | VRING_DESC_F_WRITE; // no next
    vring2.desc[1].addr = syscall_get_physical_address((uintptr_t)rdata2, NULL);
    vring2.desc[1].len = PAGE_SIZE;


    Buffer readData3(PAGE_SIZE * 2);
    const uintptr_t phys5 = syscall_get_physical_address((uintptr_t)readData3.data(), NULL);
    const uintptr_t aphys5 = (phys4+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata3 = (uint8_t *) (readData3.data() + aphys5 - phys5);

    vring2.desc[2].flags = VRING_DESC_F_WRITE; // no next
    vring2.desc[2].addr = syscall_get_physical_address((uintptr_t)rdata2, NULL);
    vring2.desc[2].len = PAGE_SIZE;
    vring2.avail->idx = 1;
    vring2.avail->ring[0] =0;

    int last_used_idx = vring2.used->idx;

    outp16(baseAddress + VIRTIO_PCI_QUEUE_NOTIFY, 0);


   while (true) {
       printf("waiting \n");
       while (vring2.used->idx == last_used_idx) {
//           sleep(20);
       }

       int next_used = vring2.used->idx;
       ASSERT(1 == next_used - last_used_idx);

       printf("used_idx %d => %d\n", last_used_idx, next_used);

       if (!(vring2.used->flags & VRING_USED_F_NO_NOTIFY)) {
           printf("*** NOTIFY ****\n");
           outp16(baseAddress + VIRTIO_PCI_QUEUE_NOTIFY, 0);
       }

    Ether::Frame* rframe = (Ether::Frame*)rdata2;
    printf("[Ether1] %x from %d:%d:%d:%d:%d:%d\n",
           Util::swapShort(rframe->type),
           rframe->srcmac[0],
           rframe->srcmac[1],
           rframe->srcmac[2],
           rframe->srcmac[3],
           rframe->srcmac[4],
           rframe->srcmac[5]
           );

    Ether::Frame* rframe2 = (Ether::Frame*)rdata3;
    printf("[Ether2] %x from %d:%d:%d:%d:%d:%d\n",
           Util::swapShort(rframe2->type),
           rframe2->srcmac[0],
           rframe2->srcmac[1],
           rframe2->srcmac[2],
           rframe2->srcmac[3],
           rframe2->srcmac[4],
           rframe2->srcmac[5]
           );


    // こうすれば desc[0] ->next-> desc[1] の組が使われるはず
    vring2.avail->ring[(vring2.avail->idx + 1) % vring2.num] = 0;
    vring2.avail->idx = (vring2.avail->idx + 1) % vring2.num;

    // これは任意。 avail-idx を変えるだけでうけとれるっぽい
    outp16(baseAddress + VIRTIO_PCI_QUEUE_NOTIFY, vring2.avail->idx);

    // 念のためバッファをクリア
    memset(rframe, 0, sizeof(Ether::Frame));
    memset(rframe2, 0, sizeof(Ether::Frame));

    last_used_idx = next_used;
   }
    }

#endif

#if 0
// OK we want to know how used index works
int main(int argc, char* argv[])
{
    // 1. Device probe
    PciInf pciInf;
    Pci pci;
    pci.CheckPciExist(PCI_VENDOR_ID_REDHAT_QUMRANET, PCI_DEVICE_ID_VIRTIO_NET, &pciInf);

    if (!pciInf.isExist) {
        printf("device not found\n");
        exit(-1);
    }

#if 0
    printf("device found\n");
    printf("baseAdress=%x\n", pciInf.baseAdress);
    printf("irqLine=%x\n", pciInf.irqLine);
#endif

    const uintptr_t baseAddress = pciInf.baseAdress & ~1;

        struct virtio_net_config config;
        for (uintptr_t i = 0; i < sizeof(config); i += 4) {
            (((uint32_t*)&config)[i / 4]) = inp32(baseAddress + VIRTIO_PCI_CONFIG + i);
        }

#if 1
        printf("config.mac= %x:%x:%x:%x:%x:%x\n", config.mac[0], config.mac[1], config.mac[2], config.mac[3], config.mac[4], config.mac[5]);
#endif

    // 2. reset the device
    outp8(baseAddress + VIRTIO_PCI_STATUS,  VIRTIO_CONFIG_S_DRIVER | VIRTIO_CONFIG_S_DRIVER_OK); // 0: reset
#if 0
    printf("[virtio] isr=%x\n", inp8(baseAddress + VIRTIO_PCI_ISR)); // clear ISR.
#endif

    // 3. IRQ receiver
    monapi_set_irq(pciInf.irqLine, MONAPI_TRUE, MONAPI_TRUE);
    syscall_set_irq_receiver(pciInf.irqLine, SYS_MASK_INTERRUPT);

// here
    // setup receive ring
    // 4. Select the queue to use
    outp16(baseAddress + VIRTIO_PCI_QUEUE_SEL, 0); // 0: read queue, 1: write queue
    // 5. how many descriptors do the queue have?
    const int numberOfDesc2 = inp16(baseAddress + VIRTIO_PCI_QUEUE_NUM);

    printf("[ARP Request] Who has 192.168.50.2? Tell 192.168.50.3\n");

#if 0

    printf("[virtio] numberOfDesc2=%d\n", numberOfDesc2);
#endif
    ASSERT(numberOfDesc2 == 256);

    // 6. Check wheter the queue is already set vring (necessary?).
    uint16_t pfn2 = inp16(baseAddress + VIRTIO_PCI_QUEUE_PFN);

    if (pfn2 != 0) {
        printf("[virtio] pfn2=%x\n", pfn2);
        exit(-1);
    }

    // 7. setup vring
    const int MAX_QUEUE_SIZE = PAGE_MASK + vring_size(MAX_QUEUE_NUM);
    Buffer readDesc(MAX_QUEUE_SIZE);
    struct vring vring2;
    vring2.num = numberOfDesc2;
    // page align is required
    const uintptr_t physicalAddress2 = syscall_get_physical_address((uintptr_t)readDesc.data(), NULL);
    const uintptr_t alignedAddress2 = (physicalAddress2 + PAGE_MASK) & ~PAGE_MASK;

    ASSERT((alignedAddress2 % PAGE_SIZE) == 0);

    // vring.desc is page aligned
    vring2.desc = (struct vring_desc*)(readDesc.data() + alignedAddress2 - physicalAddress2);

    // make linked ring
    for (uintptr_t i = 0; i < vring2.num; i++) {
        vring2.desc[i].next = i + 1;
        vring2.desc[i].flags |= VRING_DESC_F_WRITE;
    }
    vring2.desc[vring2.num - 1].next = 0;


    // vring.avail is follow after the array of desc
    vring2.avail = (struct vring_avail *)&vring2.desc[numberOfDesc2];

    // vring.used is also page aligned
    const uintptr_t usedPhysicalAddress2 = syscall_get_physical_address((uintptr_t)&(vring2.avail->ring[numberOfDesc2]), NULL);
    const uintptr_t usedAligendAddress2 = (usedPhysicalAddress2 + PAGE_MASK) & ~PAGE_MASK;
    ASSERT((usedAligendAddress2 % PAGE_SIZE) == 0);
    vring2.used = (struct vring_used*)((uintptr_t)&(vring2.avail->ring[numberOfDesc2]) + usedAligendAddress2 - usedPhysicalAddress2);

    ASSERT((uintptr_t)syscall_get_physical_address((uintptr_t)vring2.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring2.desc, NULL) == 8192);
    ASSERT((((uintptr_t)syscall_get_physical_address((uintptr_t)vring2.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring2.desc, NULL)) % PAGE_SIZE) == 0);

    // 9. set up pfn
    outp32(baseAddress + VIRTIO_PCI_QUEUE_PFN, syscall_get_physical_address((uintptr_t)vring2.desc, NULL) >> 12);

    // 10. prepare the data to write
    Buffer readData1(PAGE_SIZE * 2);
    const uintptr_t phys3 = syscall_get_physical_address((uintptr_t)readData1.data(), NULL);
    const uintptr_t aphys3 = (phys3+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata = (uint8_t *) (readData1.data() + aphys3 - phys3);

    vring2.desc[0].flags = VRING_DESC_F_NEXT | VRING_DESC_F_WRITE; // no next
    vring2.desc[0].addr = syscall_get_physical_address((uintptr_t)rdata, NULL);
    vring2.desc[0].len = sizeof(struct virtio_net_hdr);

    Buffer readData2(PAGE_SIZE * 2);
    const uintptr_t phys4 = syscall_get_physical_address((uintptr_t)readData2.data(), NULL);
    const uintptr_t aphys4 = (phys4+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata2 = (uint8_t *) (readData2.data() + aphys4 - phys4);
    vring2.desc[1].flags = VRING_DESC_F_WRITE; // no next
    vring2.desc[1].addr = syscall_get_physical_address((uintptr_t)rdata2, NULL);
    vring2.desc[1].len = PAGE_SIZE;
    vring2.avail->idx = 1;
    vring2.avail->ring[0] =0;

    int last_used_idx = vring2.used->idx;

    outp16(baseAddress + VIRTIO_PCI_QUEUE_NOTIFY, 0);

   ASSERT(vring2.desc[0].flags & VRING_DESC_F_NEXT);
   ASSERT((vring2.desc[1].flags & VRING_DESC_F_NEXT) == 0);
   ASSERT(vring2.desc[1].flags & VRING_DESC_F_WRITE);

   while (true) {
       printf("waiting \n");
       while (vring2.used->idx == last_used_idx) {
//           sleep(20);
       }

       int next_used = vring2.used->idx;
       ASSERT(1 == next_used - last_used_idx);

       printf("used_idx %d => %d\n", last_used_idx, next_used);

       if (!(vring2.used->flags & VRING_USED_F_NO_NOTIFY)) {
           printf("*** NOTIFY ****\n");
           outp16(baseAddress + VIRTIO_PCI_QUEUE_NOTIFY, 0);
       }

    Ether::Frame* rframe = (Ether::Frame*)rdata2;
    printf("[Ether] %x from %d:%d:%d:%d:%d:%d\n",
           Util::swapShort(rframe->type),
           rframe->srcmac[0],
           rframe->srcmac[1],
           rframe->srcmac[2],
           rframe->srcmac[3],
           rframe->srcmac[4],
           rframe->srcmac[5]
           );

    // こうすれば desc[0] ->next-> desc[1] の組が使われるはず
    vring2.avail->ring[(vring2.avail->idx + 1) % vring2.num] = 0;
    vring2.avail->idx = (vring2.avail->idx + 1) % vring2.num;

    // これは任意。 avail-idx を変えるだけでうけとれるっぽい
    outp16(baseAddress + VIRTIO_PCI_QUEUE_NOTIFY, vring2.avail->idx);

    // 念のためバッファをクリア
    memset(rframe, 0, sizeof(Ether::Frame));

    last_used_idx = next_used;
   }
    }
#endif

#if 0
// with -net tap,if_name=tap0
// ICMP Request can see on Wireshark !
// reply is also ok
int main(int argc, char* argv[])
{
    // 1. Device probe
    PciInf pciInf;
    Pci pci;
    pci.CheckPciExist(PCI_VENDOR_ID_REDHAT_QUMRANET, PCI_DEVICE_ID_VIRTIO_NET, &pciInf);

    if (!pciInf.isExist) {
        printf("device not found\n");
        exit(-1);
    }

#if 0
    printf("device found\n");
    printf("baseAdress=%x\n", pciInf.baseAdress);
    printf("irqLine=%x\n", pciInf.irqLine);
#endif

    const uintptr_t baseAddress = pciInf.baseAdress & ~1;

        struct virtio_net_config config;
        for (uintptr_t i = 0; i < sizeof(config); i += 4) {
            (((uint32_t*)&config)[i / 4]) = inp32(baseAddress + VIRTIO_PCI_CONFIG + i);
        }

#if 1
        printf("config.mac= %x:%x:%x:%x:%x:%x\n", config.mac[0], config.mac[1], config.mac[2], config.mac[3], config.mac[4], config.mac[5]);
#endif

    // 2. reset the device
    outp8(baseAddress + VIRTIO_PCI_STATUS,  VIRTIO_CONFIG_S_DRIVER | VIRTIO_CONFIG_S_DRIVER_OK); // 0: reset
#if 0
    printf("[virtio] isr=%x\n", inp8(baseAddress + VIRTIO_PCI_ISR)); // clear ISR.
#endif

    // 3. IRQ receiver
    monapi_set_irq(pciInf.irqLine, MONAPI_TRUE, MONAPI_TRUE);
    syscall_set_irq_receiver(pciInf.irqLine, SYS_MASK_INTERRUPT);

    // 4. Select the queue to use
    outp16(baseAddress + VIRTIO_PCI_QUEUE_SEL, 1); // 0: read queue, 1: write queue

    // 5. how many descriptors do the queue have?
    const int numberOfDesc = inp16(baseAddress + VIRTIO_PCI_QUEUE_NUM);

#if 0
    printf("[virtio] numberOfDesc=%d\n", numberOfDesc);
#endif
    ASSERT(numberOfDesc == 256);

    // 6. Check wheter the queue is already set vring (necessary?).
    uint16_t pfn = inp16(baseAddress + VIRTIO_PCI_QUEUE_PFN);
    if (pfn != 0) {
        printf("[virtio] pfn=%x\n", pfn);
        exit(-1);
    }

    // 7. setup vring
    const int MAX_QUEUE_SIZE = PAGE_MASK + vring_size(MAX_QUEUE_NUM);
#if 0
    printf("[virtio] MAX_QUEUE_SIZE=%d\n", MAX_QUEUE_SIZE);
#endif
//    uint8_t* queueData= new uint8_t[MAX_QUEUE_SIZE];
//    uint8_t queueData[MAX_QUEUE_SIZE];
    Buffer writeDesc(MAX_QUEUE_SIZE);
    CHECK_BUFFER(writeDesc);

    struct vring vring;
    vring.num = numberOfDesc;
    // page align is required
    const uintptr_t physicalAddress = syscall_get_physical_address((uintptr_t)writeDesc.data(), NULL);
#if 0
    printf("[virtio] physicalAddress=%x\n", physicalAddress);
#endif
    const uintptr_t alignedAddress = (physicalAddress + PAGE_MASK) & ~PAGE_MASK;
#if 0
    printf("[virtio] alignedAddress=%x\n", alignedAddress);
#endif
    ASSERT((alignedAddress % PAGE_SIZE) == 0);

    // vring.desc is page aligned
    vring.desc = (struct vring_desc*)(writeDesc.data() + alignedAddress - physicalAddress);
    //make linked ring
    for (uintptr_t i = 0; i < vring.num; i++) {
        vring.desc[i].next = i + 1;
        vring.desc[i].len = i + 1; // for debug. With it we can know which desc is wrong state
    }
    vring.desc[vring.num - 1].next = 0;

    // vring.avail is follow after the array of desc
    vring.avail = (struct vring_avail *)&vring.desc[numberOfDesc];
    ASSERT((syscall_get_physical_address((uintptr_t)vring.avail, NULL) % PAGE_SIZE) == 0)


    //vring.used is also page aligned
    const uintptr_t usedPhysicalAddress = syscall_get_physical_address((uintptr_t)&(vring.avail->ring[numberOfDesc]), NULL);
    const uintptr_t usedAligendAddress = (usedPhysicalAddress + PAGE_MASK) & ~PAGE_MASK;
    ASSERT((usedAligendAddress % PAGE_SIZE) == 0);
#if 0
    _printf("usedAligendAddress - usedPhysicalAddress= %d\n", usedAligendAddress - usedPhysicalAddress);
#endif
    vring.used = (struct vring_used*)((uintptr_t)&(vring.avail->ring[numberOfDesc]) + usedAligendAddress - usedPhysicalAddress);

#if 0
    printf("vring.desc=%x vring.used=%x diff=%d\n"
           , syscall_get_physical_address((uintptr_t)vring.desc, NULL)
           , syscall_get_physical_address((uintptr_t)vring.used, NULL)
           , (uintptr_t)syscall_get_physical_address((uintptr_t)vring.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring.desc, NULL));
#endif
    ASSERT((((uintptr_t)syscall_get_physical_address((uintptr_t)vring.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring.desc, NULL))) == 8192);

    ASSERT((uintptr_t)(&(vring.used->ring[numberOfDesc])) <= (uintptr_t)(writeDesc.data() + writeDesc.size()));
    // 9. set up pfn

#if 0
    printf("[virtio] set up PFN\n");
#endif
    outp32(baseAddress + VIRTIO_PCI_QUEUE_PFN, syscall_get_physical_address((uintptr_t)vring.desc, NULL) >> 12);
    //10. prepare the data to write

    Buffer writeData1(PAGE_SIZE * 2);
    const uintptr_t phys1 = syscall_get_physical_address((uintptr_t)writeData1.data(), NULL);
    const uintptr_t aphys1 = (phys1+ PAGE_MASK) & ~PAGE_MASK;
    struct virtio_net_hdr* hdr = (struct virtio_net_hdr*) (writeData1.data() + aphys1 - phys1);
    // This is *necessary*
   hdr->flags = 0;
   hdr->csum_offset = 0;
   hdr->csum_start = 0;
   hdr->gso_type = VIRTIO_NET_HDR_GSO_NONE;
   hdr->gso_size = 0;
   hdr->hdr_len = 0;
    vring.desc[0].flags  |= VRING_DESC_F_NEXT; ; // no next
    vring.desc[0].addr = syscall_get_physical_address((uintptr_t)hdr, NULL);
    vring.desc[0].len = sizeof(struct virtio_net_hdr);

    Buffer writeData2(PAGE_SIZE * 2);

    const uintptr_t phys2 = syscall_get_physical_address((uintptr_t)writeData2.data(), NULL);
    const uintptr_t aphys2 = (phys2+ PAGE_MASK) & ~PAGE_MASK;
    Ether::Frame* frame = (Ether::Frame*) (writeData2.data() + aphys2 - phys2);
    frame->dstmac[0] = 0xff;
    frame->dstmac[1] = 0xff;
    frame->dstmac[2] = 0xff;
    frame->dstmac[3] = 0xff;
    frame->dstmac[4] = 0xff;
    frame->dstmac[5] = 0xff;
    memcpy(frame->srcmac, config.mac, 6);
    frame->type = Util::swapShort(Ether::ARP);
    Arp::Header* header = (Arp::Header*)frame->data;
        header->hardType =Util::swapShort(Arp::HARD_TYPE_ETHER);
        header->protType = Util::swapShort(Arp::PROTCOL_TYPE_IP);
        header->hardAddrLen = 6;
        header->protAddrLen = 4;
        memcpy(header->srcMac, config.mac, 6);
        header->opeCode = Util::swapShort(Arp::OPE_CODE_ARP_REQ);
        header->srcIp = Util::ipAddressToDuint16_t(192, 168, 50, 3);
        // QEMU network is 10.0.2.x
        header->dstIp = Util::ipAddressToDuint16_t(192, 168, 50, 2); // this is HOST tap0
        memset(header->dstMac, 0, 6);
    // 11. set the data to vring
    vring.desc[1].flags = 0; // no next
    vring.desc[1].addr = syscall_get_physical_address((uintptr_t)frame, NULL);
    vring.desc[1].len = sizeof(Ether::Frame);
    vring.avail->idx = 1;
    vring.avail->ring[0] =0;
    // setup receive ring
    // 4. Select the queue to use
    outp16(baseAddress + VIRTIO_PCI_QUEUE_SEL, 0); // 0: read queue, 1: write queue
    // 5. how many descriptors do the queue have?
    const int numberOfDesc2 = inp16(baseAddress + VIRTIO_PCI_QUEUE_NUM);

    printf("[ARP Request] Who has 192.168.50.2? Tell 192.168.50.3\n");

#if 0

    printf("[virtio] numberOfDesc2=%d\n", numberOfDesc2);
#endif
    ASSERT(numberOfDesc2 == 256);

    // 6. Check wheter the queue is already set vring (necessary?).
    uint16_t pfn2 = inp16(baseAddress + VIRTIO_PCI_QUEUE_PFN);

    if (pfn2 != 0) {
        printf("[virtio] pfn2=%x\n", pfn2);
        exit(-1);
    }

    // 7. setup vring
//    uint8_t*  queueData2 = new uint8_t[MAX_QUEUE_SIZE];
//    uint8_t  queueData2[MAX_QUEUE_SIZE];
//    memset(queueData2, 0, MAX_QUEUE_SIZE);

    Buffer readDesc(MAX_QUEUE_SIZE);
    struct vring vring2;
    vring2.num = numberOfDesc2;
    // page align is required
    const uintptr_t physicalAddress2 = syscall_get_physical_address((uintptr_t)readDesc.data(), NULL);
//    printf("[virtio] physicalAddress2=%x\n", physicalAddress2);
    const uintptr_t alignedAddress2 = (physicalAddress2 + PAGE_MASK) & ~PAGE_MASK;
//    printf("[virtio] alignedAddress2=%x\n", alignedAddress2);

    ASSERT((alignedAddress2 % PAGE_SIZE) == 0);

    // vring.desc is page aligned
    vring2.desc = (struct vring_desc*)(readDesc.data() + alignedAddress2 - physicalAddress2);

    // make linked ring
    for (uintptr_t i = 0; i < vring2.num; i++) {
        vring2.desc[i].next = i + 1;
        vring2.desc[i].flags |= VRING_DESC_F_WRITE;
    }
    vring2.desc[vring2.num - 1].next = 0;


    // vring.avail is follow after the array of desc
    vring2.avail = (struct vring_avail *)&vring2.desc[numberOfDesc2];

    // vring.used is also page aligned
    const uintptr_t usedPhysicalAddress2 = syscall_get_physical_address((uintptr_t)&(vring2.avail->ring[numberOfDesc2]), NULL);
    const uintptr_t usedAligendAddress2 = (usedPhysicalAddress2 + PAGE_MASK) & ~PAGE_MASK;
    ASSERT((usedAligendAddress2 % PAGE_SIZE) == 0);
    vring2.used = (struct vring_used*)((uintptr_t)&(vring2.avail->ring[numberOfDesc2]) + usedAligendAddress2 - usedPhysicalAddress2);

//     printf("vring2.desc=%x vring2.used=%x diff=%d\n"
//            , syscall_get_physical_address((uintptr_t)vring2.desc, NULL)
//            , syscall_get_physical_address((uintptr_t)vring2.used, NULL)
//            , (uintptr_t)syscall_get_physical_address((uintptr_t)vring2.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring2.desc, NULL));

    ASSERT((uintptr_t)syscall_get_physical_address((uintptr_t)vring2.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring2.desc, NULL) == 8192);
    ASSERT((((uintptr_t)syscall_get_physical_address((uintptr_t)vring2.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring2.desc, NULL)) % PAGE_SIZE) == 0);

    // 9. set up pfn
//    printf("[virtio] set up PFN\n");
    outp32(baseAddress + VIRTIO_PCI_QUEUE_PFN, syscall_get_physical_address((uintptr_t)vring2.desc, NULL) >> 12);

    // 10. prepare the data to write
//    uint8_t data3[PAGE_SIZE * 2];
    Buffer readData1(PAGE_SIZE * 2);
    const uintptr_t phys3 = syscall_get_physical_address((uintptr_t)readData1.data(), NULL);
    const uintptr_t aphys3 = (phys3+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata = (uint8_t *) (readData1.data() + aphys3 - phys3);

    vring2.desc[0].flags = VRING_DESC_F_NEXT|VRING_DESC_F_WRITE; // no next
    vring2.desc[0].addr = syscall_get_physical_address((uintptr_t)rdata, NULL);
    vring2.desc[0].len = sizeof(struct virtio_net_hdr);

    Buffer readData2(PAGE_SIZE * 2);
    const uintptr_t phys4 = syscall_get_physical_address((uintptr_t)readData2.data(), NULL);
    const uintptr_t aphys4 = (phys4+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata2 = (uint8_t *) (readData2.data() + aphys4 - phys4);
    vring2.desc[1].flags = VRING_DESC_F_WRITE; // no next
    vring2.desc[1].addr = syscall_get_physical_address((uintptr_t)rdata2, NULL);
    vring2.desc[1].len = PAGE_SIZE;
    vring2.avail->idx = 1;
    vring2.avail->ring[0] =0;

    outp16(baseAddress + VIRTIO_PCI_QUEUE_NOTIFY, 0);
    ASSERT(vring.desc[0].flags == VRING_DESC_F_NEXT);
    ASSERT(vring.desc[1].flags == 0);

    ASSERT(vring2.desc[0].flags & VRING_DESC_F_NEXT);
    ASSERT((vring2.desc[1].flags & VRING_DESC_F_NEXT) == 0);
    ASSERT(vring2.desc[1].flags & VRING_DESC_F_WRITE);


    CHECK_BUFFER(writeDesc);
    CHECK_BUFFER(writeData1);
    CHECK_BUFFER(writeData2);
    CHECK_BUFFER(readDesc);
    CHECK_BUFFER(readData1);
    CHECK_BUFFER(readData2);


    // 12. Notify!
    //  printf("[virtio] vring.used->idx = %d \n", vring.used->idx);
    outp16(baseAddress + VIRTIO_PCI_QUEUE_NOTIFY, 1);

    while (vring.used->idx == 0) {
        sleep(300);
        printf("waiting \n");
    }

    if (!(vring2.used->flags & VRING_USED_F_NO_NOTIFY)) {
        printf("*** NOTIFY ****\n");
        outp16(baseAddress + VIRTIO_PCI_QUEUE_NOTIFY, 0);
    }

//     // 13. wait notification from host
//     MessageInfo msg;
//     Message::receive(&msg);

//     switch (msg.header)
//     {
//     case MSG_INTERRUPTED:
//     {
// //        printf("[virtio] Interrupt comes\n");
//         // clear ISR.
//         const uint8_t isr = inp8(baseAddress + VIRTIO_PCI_ISR);
// //        printf("[virtio] isr=%x\n", isr);
//         monapi_set_irq(pciInf.irqLine, MONAPI_TRUE, MONAPI_TRUE);
//         CHECK_BUFFER(writeDesc);
//         CHECK_BUFFER(writeData1);
//         CHECK_BUFFER(writeData2);
//         CHECK_BUFFER(readDesc);
//         CHECK_BUFFER(readData1);
//         CHECK_BUFFER(readData2);
//         break;
//     }
//     default:
//         printf("[virtio] uknown message\n");
//         break;
//     }

    // if data is written, idx 0 => 1
//     printf("[virtio] vring.used->idx = %d \n", vring.used->idx);

//     printf("***** send done ****");
//     printf("[virtio] vring2.used->idx = %d \n", vring2.used->idx);

//    sleep(1000);

    while (vring2.used->idx == 0) {
        sleep(300);
        printf("waiting \n");
    }
    CHECK_BUFFER(writeDesc);
    CHECK_BUFFER(writeData1);
    CHECK_BUFFER(writeData2);
    CHECK_BUFFER(readDesc);
    CHECK_BUFFER(readData1);
    CHECK_BUFFER(readData2);

    Ether::Frame* rframe = (Ether::Frame*)rdata2;
//    printf("rframe->type=%x\n", rframe->type);

    if (rframe->type == Util::swapShort(Ether::ARP)) {
        Arp::Header* hdr = (Arp::Header*)rframe->data;

        printf("[ARP Reply  ] %s is %x:%x:%x:%x:%x:%x\n"
               , (const char*)Util::ipAddressToCString(hdr->srcIp)
               , hdr->srcMac[0]
               , hdr->srcMac[1]
               , hdr->srcMac[2]
               , hdr->srcMac[3]
               , hdr->srcMac[4]
               , hdr->srcMac[5]);

        printf("[ICMP REQ] to 192.168.50.2\n");

        frame->type =  Util::swapShort(Ether::IP);
        memcpy(frame->dstmac, hdr->srcMac, 6);
        memcpy(frame->srcmac, config.mac, 6);
        IP::Header* ipHeader = (IP::Header*)frame->data;
    ipHeader->verhead = (4 << 4) | (sizeof(IP::Header) / 4);
    ipHeader->tos    = 0;
    ipHeader->id     = 0;
    ipHeader->ttl    = 255;
    ipHeader->prot   = IP::ICMP;
    ipHeader->srcip  = Util::ipAddressToDuint16_t(192, 168, 50, 3);
    ipHeader->dstip  = Util::ipAddressToDuint16_t(192, 168, 50, 2);
    ipHeader->chksum = 0;
    ipHeader->len    = Util::swapShort(sizeof(IP::Header) + sizeof(Icmp::Header));
    ipHeader->frag   = 0;
    ipHeader->chksum = Util::calcCheckSum((uint32_t*)ipHeader, sizeof(IP::Header));

    Icmp::Header* icmp = (Icmp::Header*)(ipHeader->data);
    icmp->type = 8;
    icmp->code = 0;
    icmp->chksum = 0;
    memset(icmp->data, 0, 32);
    icmp->chksum = Util::calcCheckSum((uint32_t*)icmp, sizeof(Icmp::Header));


    printf("before 2nd write vring.avail->idx=%x\n", vring.avail->idx);
    printf("before 2nd write vring.used->idx=%x\n", vring.used->idx);


//     vring.avail->idx = 1;
//     vring.avail->ring[0] =0;
//     vring.used->idx  = 0;

    vring2.desc[2].flags = VRING_DESC_F_NEXT|VRING_DESC_F_WRITE; // no next
    vring2.desc[2].addr = syscall_get_physical_address((uintptr_t)rdata, NULL);
    vring2.desc[2].len = sizeof(struct virtio_net_hdr);

    Buffer readData2(PAGE_SIZE * 2);
    const uintptr_t phys4 = syscall_get_physical_address((uintptr_t)readData2.data(), NULL);
    const uintptr_t aphys4 = (phys4+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata2 = (uint8_t *) (readData2.data() + aphys4 - phys4);
    vring2.desc[3].flags = VRING_DESC_F_WRITE; // no next
    vring2.desc[3].addr = syscall_get_physical_address((uintptr_t)rdata2, NULL);
    vring2.desc[3].len = PAGE_SIZE;
    vring2.avail->idx = 3;
    vring2.used->idx = 0;
    vring2.avail->ring[2] =2;


    vring.desc[2].flags  |= VRING_DESC_F_NEXT; ; // no next
    vring.desc[2].addr = syscall_get_physical_address((uintptr_t)hdr, NULL);
    vring.desc[2].len = sizeof(struct virtio_net_hdr);

    vring.desc[3].flags = 0; // no next
    vring.desc[3].addr = syscall_get_physical_address((uintptr_t)frame, NULL);
    vring.desc[3].len = sizeof(Ether::Frame);
    vring.avail->idx = 3;
    vring.avail->ring[1] =2;


//     outp16(baseAddress + VIRTIO_PCI_QUEUE_NOTIFY, 0);
//     memset(writeData2.data(), 0, PAGE_SIZE * 2);
//     memset(rdata2, 0, PAGE_SIZE);
     outp16(baseAddress + VIRTIO_PCI_QUEUE_NOTIFY, 1);

    MessageInfo msg;
    Message::receive(&msg);
    switch (msg.header)
    {
    case MSG_INTERRUPTED:
    {
        printf("[virtio] Interrupt comes2\n");
        // clear ISR.
        const uint8_t isr = inp8(baseAddress + VIRTIO_PCI_ISR);
//        printf("[virtio] isr=%x\n", isr);
        monapi_set_irq(pciInf.irqLine, MONAPI_TRUE, MONAPI_TRUE);
        CHECK_BUFFER(writeDesc);
        CHECK_BUFFER(writeData1);
        CHECK_BUFFER(writeData2);
        CHECK_BUFFER(readDesc);
        CHECK_BUFFER(readData1);
        CHECK_BUFFER(readData2);
        break;
    }
    default:
        printf("[virtio] uknown message\n");
        break;
    }

//    do {
        while (vring2.used->idx == 1) {
            sleep(300);
            printf("waiting \n");
        }


    if (!(vring2.used->flags & VRING_USED_F_NO_NOTIFY)) {
        printf("*** NOTIFY ****\n");
        outp16(baseAddress + VIRTIO_PCI_QUEUE_NOTIFY, 0);
    }
    uint8_t* rdata3 = (uint8_t *) (readData2.data() + aphys4 - phys4);

    printf("vring2.used->idx=%x vr->used->flags=%d\n", vring2.used->idx, vring2.used->flags);
        Ether::Frame* rframe2 = (Ether::Frame*)rdata3;
        if (rframe2->type == Util::swapShort(Ether::IP)) {
            IP::Header* ih = (IP::Header*)rframe2->data;

            printf("srciP=%s\n", (const char*)Util::ipAddressToCString(ih->srcip));
            if (ih->prot == IP::ICMP) {
                printf("[ICMP] reply\n");
//                ICMP::header* icmp = (ICMP::header)ih->data
            } else {
                printf("not icmp %s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
            }
        } else {
            printf("%x %s %s:%d\n", rframe->type, __func__, __FILE__, __LINE__);fflush(stdout);// debug
        }
//    }

//     vring2.avail->idx = 1;
//     vring2.avail->ring[0] =0;
//     vring2.used->idx == 0;
//     memset(writeData2.data(), 0, PAGE_SIZE * 2);
//             printf("%x %s %s:%d\n", rframe->type, __func__, __FILE__, __LINE__);fflush(stdout);// debug
// //    outp16(baseAddress + VIRTIO_PCI_QUEUE_NOTIFY, 1);

//     sleep(2000);
//     } while (true);

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


//         printf("hardType=%x\n", hdr->hardType);
//         printf("protType=%x\n", hdr->protType);
//         printf("hardAddrLen=%x\n", hdr->hardAddrLen);
//         printf("protAddrLen=%x\n", hdr->protAddrLen);
//         printf("opeCode =%x\n", hdr->opeCode );
//         for (int i = 0; i < 6 ; i++) {
//             printf("srcMac=[i]=%x\n", hdr->srcMac[i]);

//         }
//         printf("  srcIp=%x\n",   hdr->srcIp);
//        printf("dstMac[6]=%x\n", dstMac[6]);

//     } else {
// //        printf("rframe->type=%d \n", rframe->type);
//     }



//    printf("[virtio] vring2.used->idx = %d \n", vring2.used->idx);
//     {
//     MessageInfo msg;
//     Message::receive(&msg);

//     switch (msg.header)
//     {
//     case MSG_INTERRUPTED:
//     {
//         printf("[virtio] Interrupt comes\n");
//         // clear ISR.
//         const uint8_t isr = inp8(baseAddress + VIRTIO_PCI_ISR);
//         printf("[virtio] isr=%x\n", isr);
//         monapi_set_irq(pciInf.irqLine, MONAPI_TRUE, MONAPI_TRUE);
//         break;
//     }
//     default:
//         printf("[virtio] uknown message\n");
//         break;
//     }
//     }
//     printf("[virtio] vring2.used->idx = %d \n", vring2.used->idx);

//     CHECK_BUFFER(writeDesc);
//     CHECK_BUFFER(writeData1);
//     CHECK_BUFFER(writeData2);
//     CHECK_BUFFER(readDesc);
//     CHECK_BUFFER(readData1);
//     CHECK_BUFFER(readData2);

    }}

#endif

#if 0

// with -net user
// ICMP Request can see on Wireshark !
int main(int argc, char* argv[])
{
    // 1. Device probe
    PciInf pciInf;
    Pci pci;
    pci.CheckPciExist(PCI_VENDOR_ID_REDHAT_QUMRANET, PCI_DEVICE_ID_VIRTIO_NET, &pciInf);

    if (!pciInf.isExist) {
        printf("device not found\n");
        exit(-1);
    }

#if 0
    printf("device found\n");
    printf("baseAdress=%x\n", pciInf.baseAdress);
    printf("irqLine=%x\n", pciInf.irqLine);
#endif

    const uintptr_t baseAddress = pciInf.baseAdress & ~1;

        struct virtio_net_config config;
        for (uintptr_t i = 0; i < sizeof(config); i += 4) {
            (((uint32_t*)&config)[i / 4]) = inp32(baseAddress + VIRTIO_PCI_CONFIG + i);
        }

#if 0
        printf("config.mac= %x:%x:%x:%x:%x:%x\n", config.mac[0], config.mac[1], config.mac[2], config.mac[3], config.mac[4], config.mac[5]);
#endif

    // 2. reset the device
    outp8(baseAddress + VIRTIO_PCI_STATUS,  VIRTIO_CONFIG_S_DRIVER | VIRTIO_CONFIG_S_DRIVER_OK); // 0: reset
#if 0
    printf("[virtio] isr=%x\n", inp8(baseAddress + VIRTIO_PCI_ISR)); // clear ISR.
#endif

    // 3. IRQ receiver
    monapi_set_irq(pciInf.irqLine, MONAPI_TRUE, MONAPI_TRUE);
    syscall_set_irq_receiver(pciInf.irqLine, SYS_MASK_INTERRUPT);

    // 4. Select the queue to use
    outp16(baseAddress + VIRTIO_PCI_QUEUE_SEL, 1); // 0: read queue, 1: write queue

    // 5. how many descriptors do the queue have?
    const int numberOfDesc = inp16(baseAddress + VIRTIO_PCI_QUEUE_NUM);

#if 0
    printf("[virtio] numberOfDesc=%d\n", numberOfDesc);
#endif
    ASSERT(numberOfDesc == 256);

    // 6. Check wheter the queue is already set vring (necessary?).
    uint16_t pfn = inp16(baseAddress + VIRTIO_PCI_QUEUE_PFN);
    if (pfn != 0) {
        printf("[virtio] pfn=%x\n", pfn);
        exit(-1);
    }

    // 7. setup vring
    const int MAX_QUEUE_SIZE = PAGE_MASK + vring_size(MAX_QUEUE_NUM);
#if 0
    printf("[virtio] MAX_QUEUE_SIZE=%d\n", MAX_QUEUE_SIZE);
#endif
//    uint8_t* queueData= new uint8_t[MAX_QUEUE_SIZE];
//    uint8_t queueData[MAX_QUEUE_SIZE];
    Buffer writeDesc(MAX_QUEUE_SIZE);
    CHECK_BUFFER(writeDesc);

    struct vring vring;
    vring.num = numberOfDesc;
    // page align is required
    const uintptr_t physicalAddress = syscall_get_physical_address((uintptr_t)writeDesc.data(), NULL);
#if 0
    printf("[virtio] physicalAddress=%x\n", physicalAddress);
#endif
    const uintptr_t alignedAddress = (physicalAddress + PAGE_MASK) & ~PAGE_MASK;
#if 0
    printf("[virtio] alignedAddress=%x\n", alignedAddress);
#endif
    ASSERT((alignedAddress % PAGE_SIZE) == 0);

    // vring.desc is page aligned
    vring.desc = (struct vring_desc*)(writeDesc.data() + alignedAddress - physicalAddress);
    //make linked ring
    for (uintptr_t i = 0; i < vring.num; i++) {
        vring.desc[i].next = i + 1;
        vring.desc[i].len = i + 1; // for debug. With it we can know which desc is wrong state
    }
    vring.desc[vring.num - 1].next = 0;

    // vring.avail is follow after the array of desc
    vring.avail = (struct vring_avail *)&vring.desc[numberOfDesc];
    ASSERT((syscall_get_physical_address((uintptr_t)vring.avail, NULL) % PAGE_SIZE) == 0)


    //vring.used is also page aligned
    const uintptr_t usedPhysicalAddress = syscall_get_physical_address((uintptr_t)&(vring.avail->ring[numberOfDesc]), NULL);
    const uintptr_t usedAligendAddress = (usedPhysicalAddress + PAGE_MASK) & ~PAGE_MASK;
    ASSERT((usedAligendAddress % PAGE_SIZE) == 0);
#if 0
    _printf("usedAligendAddress - usedPhysicalAddress= %d\n", usedAligendAddress - usedPhysicalAddress);
#endif
    vring.used = (struct vring_used*)((uintptr_t)&(vring.avail->ring[numberOfDesc]) + usedAligendAddress - usedPhysicalAddress);

#if 0
    printf("vring.desc=%x vring.used=%x diff=%d\n"
           , syscall_get_physical_address((uintptr_t)vring.desc, NULL)
           , syscall_get_physical_address((uintptr_t)vring.used, NULL)
           , (uintptr_t)syscall_get_physical_address((uintptr_t)vring.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring.desc, NULL));
#endif
    ASSERT((((uintptr_t)syscall_get_physical_address((uintptr_t)vring.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring.desc, NULL))) == 8192);

    ASSERT((uintptr_t)(&(vring.used->ring[numberOfDesc])) <= (uintptr_t)(writeDesc.data() + writeDesc.size()));
    // 9. set up pfn

#if 0
    printf("[virtio] set up PFN\n");
#endif
    outp32(baseAddress + VIRTIO_PCI_QUEUE_PFN, syscall_get_physical_address((uintptr_t)vring.desc, NULL) >> 12);
    //10. prepare the data to write

    Buffer writeData1(PAGE_SIZE * 2);
    const uintptr_t phys1 = syscall_get_physical_address((uintptr_t)writeData1.data(), NULL);
    const uintptr_t aphys1 = (phys1+ PAGE_MASK) & ~PAGE_MASK;
    struct virtio_net_hdr* hdr = (struct virtio_net_hdr*) (writeData1.data() + aphys1 - phys1);
    // This is *necessary*
   hdr->flags = 0;
   hdr->csum_offset = 0;
   hdr->csum_start = 0;
   hdr->gso_type = VIRTIO_NET_HDR_GSO_NONE;
   hdr->gso_size = 0;
   hdr->hdr_len = 0;
    vring.desc[0].flags  |= VRING_DESC_F_NEXT; ; // no next
    vring.desc[0].addr = syscall_get_physical_address((uintptr_t)hdr, NULL);
    vring.desc[0].len = sizeof(struct virtio_net_hdr);

    Buffer writeData2(PAGE_SIZE * 2);

    const uintptr_t phys2 = syscall_get_physical_address((uintptr_t)writeData2.data(), NULL);
    const uintptr_t aphys2 = (phys2+ PAGE_MASK) & ~PAGE_MASK;
    Ether::Frame* frame = (Ether::Frame*) (writeData2.data() + aphys2 - phys2);

    // QEMU 10.0.2.2 Gateway mac address (todo arp)
    frame->dstmac[0] = 82;
    frame->dstmac[1] = 84;
    frame->dstmac[2] = 0;
    frame->dstmac[3] = 18;
    frame->dstmac[4] = 53;
    frame->dstmac[5] = 2;
    memcpy(frame->srcmac, config.mac, 6);
    frame->type = Util::swapShort(Ether::IP);
    IP::Header* ipHeader = (IP::Header*)frame->data;
    ipHeader->verhead = (4 << 4) | (sizeof(IP::Header) / 4);
    ipHeader->tos    = 0;
    ipHeader->id     = 0;
    ipHeader->ttl    = 255;
    ipHeader->prot   = IP::ICMP;
    ipHeader->srcip  = Util::ipAddressToDuint16_t(10, 0, 2, 10);
    ipHeader->dstip  = Util::ipAddressToDuint16_t(72, 14, 235, 104);
    ipHeader->chksum = 0;
    ipHeader->len    = Util::swapShort(sizeof(IP::Header) + sizeof(Icmp::Header));
    ipHeader->frag   = 0;
    ipHeader->chksum = Util::calcCheckSum((uint32_t*)ipHeader, sizeof(IP::Header));

    Icmp::Header* icmp = (Icmp::Header*)(ipHeader->data);
    icmp->type = 8;
    icmp->code = 0;
    icmp->chksum = 0;
    memset(icmp->data, 0, 32);
    icmp->chksum = Util::calcCheckSum((uint32_t*)icmp, sizeof(Icmp::Header));
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


//         header->hardType =Util::swapShort(Arp::HARD_TYPE_ETHER);
//         header->protType = Util::swapShort(Arp::PROTCOL_TYPE_IP);
//         header->hardAddrLen = 6;
//         header->protAddrLen = 4;
//         memcpy(header->srcMac, config.mac, 6);
//         header->opeCode = Util::swapShort(Arp::OPE_CODE_ARP_REQ);
//         header->srcIp = Util::ipAddressToDuint16_t(10, 0, 2, 10);
//         // QEMU network is 10.0.2.x
//         header->dstIp = Util::ipAddressToDuint16_t(10, 0, 2, 3); // 10.0.2.3 is user mode QEMU DNS
//         memset(header->dstMac, 0, 6);
    // 11. set the data to vring
    vring.desc[1].flags = 0; // no next
    vring.desc[1].addr = syscall_get_physical_address((uintptr_t)frame, NULL);
    vring.desc[1].len = sizeof(Ether::Frame);
    vring.avail->idx = 1;
    vring.avail->ring[0] =0;
    // setup receive ring
    // 4. Select the queue to use
    outp16(baseAddress + VIRTIO_PCI_QUEUE_SEL, 0); // 0: read queue, 1: write queue
    // 5. how many descriptors do the queue have?
    const int numberOfDesc2 = inp16(baseAddress + VIRTIO_PCI_QUEUE_NUM);

    printf("[ARP Request] Who has 10.0.2.3? Tell 10.0.2.10\n");

#if 0

    printf("[virtio] numberOfDesc2=%d\n", numberOfDesc2);
#endif
    ASSERT(numberOfDesc2 == 256);

    // 6. Check wheter the queue is already set vring (necessary?).
    uint16_t pfn2 = inp16(baseAddress + VIRTIO_PCI_QUEUE_PFN);

    if (pfn2 != 0) {
        printf("[virtio] pfn2=%x\n", pfn2);
        exit(-1);
    }

    // 7. setup vring
//    uint8_t*  queueData2 = new uint8_t[MAX_QUEUE_SIZE];
//    uint8_t  queueData2[MAX_QUEUE_SIZE];
//    memset(queueData2, 0, MAX_QUEUE_SIZE);

    Buffer readDesc(MAX_QUEUE_SIZE);
    struct vring vring2;
    vring2.num = numberOfDesc2;
    // page align is required
    const uintptr_t physicalAddress2 = syscall_get_physical_address((uintptr_t)readDesc.data(), NULL);
//    printf("[virtio] physicalAddress2=%x\n", physicalAddress2);
    const uintptr_t alignedAddress2 = (physicalAddress2 + PAGE_MASK) & ~PAGE_MASK;
//    printf("[virtio] alignedAddress2=%x\n", alignedAddress2);

    ASSERT((alignedAddress2 % PAGE_SIZE) == 0);

    // vring.desc is page aligned
    vring2.desc = (struct vring_desc*)(readDesc.data() + alignedAddress2 - physicalAddress2);

    // make linked ring
    for (uintptr_t i = 0; i < vring2.num; i++) {
        vring2.desc[i].next = i + 1;
        vring2.desc[i].flags |= VRING_DESC_F_WRITE;
    }
    vring2.desc[vring2.num - 1].next = 0;


    // vring.avail is follow after the array of desc
    vring2.avail = (struct vring_avail *)&vring2.desc[numberOfDesc2];

    // vring.used is also page aligned
    const uintptr_t usedPhysicalAddress2 = syscall_get_physical_address((uintptr_t)&(vring2.avail->ring[numberOfDesc2]), NULL);
    const uintptr_t usedAligendAddress2 = (usedPhysicalAddress2 + PAGE_MASK) & ~PAGE_MASK;
    ASSERT((usedAligendAddress2 % PAGE_SIZE) == 0);
    vring2.used = (struct vring_used*)((uintptr_t)&(vring2.avail->ring[numberOfDesc2]) + usedAligendAddress2 - usedPhysicalAddress2);

//     printf("vring2.desc=%x vring2.used=%x diff=%d\n"
//            , syscall_get_physical_address((uintptr_t)vring2.desc, NULL)
//            , syscall_get_physical_address((uintptr_t)vring2.used, NULL)
//            , (uintptr_t)syscall_get_physical_address((uintptr_t)vring2.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring2.desc, NULL));

    ASSERT((uintptr_t)syscall_get_physical_address((uintptr_t)vring2.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring2.desc, NULL) == 8192);
    ASSERT((((uintptr_t)syscall_get_physical_address((uintptr_t)vring2.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring2.desc, NULL)) % PAGE_SIZE) == 0);

    // 9. set up pfn
//    printf("[virtio] set up PFN\n");
    outp32(baseAddress + VIRTIO_PCI_QUEUE_PFN, syscall_get_physical_address((uintptr_t)vring2.desc, NULL) >> 12);

    // 10. prepare the data to write
//    uint8_t data3[PAGE_SIZE * 2];
    Buffer readData1(PAGE_SIZE * 2);
    const uintptr_t phys3 = syscall_get_physical_address((uintptr_t)readData1.data(), NULL);
    const uintptr_t aphys3 = (phys3+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata = (uint8_t *) (readData1.data() + aphys3 - phys3);

    vring2.desc[0].flags = VRING_DESC_F_NEXT|VRING_DESC_F_WRITE; // no next
    vring2.desc[0].addr = syscall_get_physical_address((uintptr_t)rdata, NULL);
    vring2.desc[0].len = sizeof(struct virtio_net_hdr);

    Buffer readData2(PAGE_SIZE * 2);
    const uintptr_t phys4 = syscall_get_physical_address((uintptr_t)readData2.data(), NULL);
    const uintptr_t aphys4 = (phys4+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata2 = (uint8_t *) (readData2.data() + aphys4 - phys4);
    vring2.desc[1].flags = VRING_DESC_F_WRITE; // no next
    vring2.desc[1].addr = syscall_get_physical_address((uintptr_t)rdata2, NULL);
    vring2.desc[1].len = PAGE_SIZE;
    vring2.avail->idx = 1;
    vring2.avail->ring[0] =0;

    outp16(baseAddress + VIRTIO_PCI_QUEUE_NOTIFY, 0);
    ASSERT(vring.desc[0].flags == VRING_DESC_F_NEXT);
    ASSERT(vring.desc[1].flags == 0);

    ASSERT(vring2.desc[0].flags & VRING_DESC_F_NEXT);
    ASSERT((vring2.desc[1].flags & VRING_DESC_F_NEXT) == 0);
    ASSERT(vring2.desc[1].flags & VRING_DESC_F_WRITE);


    CHECK_BUFFER(writeDesc);
    CHECK_BUFFER(writeData1);
    CHECK_BUFFER(writeData2);
    CHECK_BUFFER(readDesc);
    CHECK_BUFFER(readData1);
    CHECK_BUFFER(readData2);
    // 12. Notify!
    //  printf("[virtio] vring.used->idx = %d \n", vring.used->idx);
    outp16(baseAddress + VIRTIO_PCI_QUEUE_NOTIFY, 1);
    // 13. wait notification from host
    MessageInfo msg;
    Message::receive(&msg);

    switch (msg.header)
    {
    case MSG_INTERRUPTED:
    {
//        printf("[virtio] Interrupt comes\n");
        // clear ISR.
        const uint8_t isr = inp8(baseAddress + VIRTIO_PCI_ISR);
//        printf("[virtio] isr=%x\n", isr);
        monapi_set_irq(pciInf.irqLine, MONAPI_TRUE, MONAPI_TRUE);
        CHECK_BUFFER(writeDesc);
        CHECK_BUFFER(writeData1);
        CHECK_BUFFER(writeData2);
        CHECK_BUFFER(readDesc);
        CHECK_BUFFER(readData1);
        CHECK_BUFFER(readData2);
        break;
    }
    default:
        printf("[virtio] uknown message\n");
        break;
    }

    // if data is written, idx 0 => 1
//     printf("[virtio] vring.used->idx = %d \n", vring.used->idx);

//     printf("***** send done ****");
//     printf("[virtio] vring2.used->idx = %d \n", vring2.used->idx);

//    sleep(1000);

    while (vring2.used->idx == 0) {
        sleep(300);
        printf("waiting \n");
    }
    CHECK_BUFFER(writeDesc);
    CHECK_BUFFER(writeData1);
    CHECK_BUFFER(writeData2);
    CHECK_BUFFER(readDesc);
    CHECK_BUFFER(readData1);
    CHECK_BUFFER(readData2);

    Ether::Frame* rframe = (Ether::Frame*)rdata2;
//    printf("rframe->type=%x\n", rframe->type);

    if (rframe->type == Util::swapShort(Ether::ARP)) {
        Arp::Header* hdr = (Arp::Header*)rframe->data;

        printf("[ARP Reply  ] %s is %d:%d:%d:%d:%d:%d\n"
               , (const char*)Util::ipAddressToCString(hdr->srcIp)
               , hdr->srcMac[0]
               , hdr->srcMac[1]
               , hdr->srcMac[2]
               , hdr->srcMac[3]
               , hdr->srcMac[4]
               , hdr->srcMac[5]);
//         printf("hardType=%x\n", hdr->hardType);
//         printf("protType=%x\n", hdr->protType);
//         printf("hardAddrLen=%x\n", hdr->hardAddrLen);
//         printf("protAddrLen=%x\n", hdr->protAddrLen);
//         printf("opeCode =%x\n", hdr->opeCode );
//         for (int i = 0; i < 6 ; i++) {
//             printf("srcMac=[i]=%x\n", hdr->srcMac[i]);

//         }
//         printf("  srcIp=%x\n",   hdr->srcIp);
//        printf("dstMac[6]=%x\n", dstMac[6]);

    } else {
//        printf("rframe->type=%d \n", rframe->type);
    }

//    printf("[virtio] vring2.used->idx = %d \n", vring2.used->idx);
    {
    MessageInfo msg;
    Message::receive(&msg);

    switch (msg.header)
    {
    case MSG_INTERRUPTED:
    {
        printf("[virtio] Interrupt comes\n");
        // clear ISR.
        const uint8_t isr = inp8(baseAddress + VIRTIO_PCI_ISR);
        printf("[virtio] isr=%x\n", isr);
        monapi_set_irq(pciInf.irqLine, MONAPI_TRUE, MONAPI_TRUE);
        break;
    }
    default:
        printf("[virtio] uknown message\n");
        break;
    }
    }
    printf("[virtio] vring2.used->idx = %d \n", vring2.used->idx);

    CHECK_BUFFER(writeDesc);
    CHECK_BUFFER(writeData1);
    CHECK_BUFFER(writeData2);
    CHECK_BUFFER(readDesc);
    CHECK_BUFFER(readData1);
    CHECK_BUFFER(readData2);

}
#endif

#if 0
int main(int argc, char* argv[])
{
    // 1. Device probe
    PciInf pciInf;
    Pci pci;
    pci.CheckPciExist(PCI_VENDOR_ID_REDHAT_QUMRANET, PCI_DEVICE_ID_VIRTIO_NET, &pciInf);

    if (!pciInf.isExist) {
        printf("device not found\n");
        exit(-1);
    }

#if 0
    printf("device found\n");
    printf("baseAdress=%x\n", pciInf.baseAdress);
    printf("irqLine=%x\n", pciInf.irqLine);
#endif

    const uintptr_t baseAddress = pciInf.baseAdress & ~1;

        struct virtio_net_config config;
        for (uintptr_t i = 0; i < sizeof(config); i += 4) {
            (((uint32_t*)&config)[i / 4]) = inp32(baseAddress + VIRTIO_PCI_CONFIG + i);
        }

#if 0
        printf("config.mac= %x:%x:%x:%x:%x:%x\n", config.mac[0], config.mac[1], config.mac[2], config.mac[3], config.mac[4], config.mac[5]);
#endif

    // 2. reset the device
    outp8(baseAddress + VIRTIO_PCI_STATUS,  VIRTIO_CONFIG_S_DRIVER | VIRTIO_CONFIG_S_DRIVER_OK); // 0: reset
#if 0
    printf("[virtio] isr=%x\n", inp8(baseAddress + VIRTIO_PCI_ISR)); // clear ISR.
#endif

    // 3. IRQ receiver
    monapi_set_irq(pciInf.irqLine, MONAPI_TRUE, MONAPI_TRUE);
    syscall_set_irq_receiver(pciInf.irqLine, SYS_MASK_INTERRUPT);

    // 4. Select the queue to use
    outp16(baseAddress + VIRTIO_PCI_QUEUE_SEL, 1); // 0: read queue, 1: write queue

    // 5. how many descriptors do the queue have?
    const int numberOfDesc = inp16(baseAddress + VIRTIO_PCI_QUEUE_NUM);

#if 0
    printf("[virtio] numberOfDesc=%d\n", numberOfDesc);
#endif
    ASSERT(numberOfDesc == 256);

    // 6. Check wheter the queue is already set vring (necessary?).
    uint16_t pfn = inp16(baseAddress + VIRTIO_PCI_QUEUE_PFN);
    if (pfn != 0) {
        printf("[virtio] pfn=%x\n", pfn);
        exit(-1);
    }

    // 7. setup vring
    const int MAX_QUEUE_SIZE = PAGE_MASK + vring_size(MAX_QUEUE_NUM);
#if 0
    printf("[virtio] MAX_QUEUE_SIZE=%d\n", MAX_QUEUE_SIZE);
#endif
//    uint8_t* queueData= new uint8_t[MAX_QUEUE_SIZE];
//    uint8_t queueData[MAX_QUEUE_SIZE];
    Buffer writeDesc(MAX_QUEUE_SIZE);
    CHECK_BUFFER(writeDesc);

    struct vring vring;
    vring.num = numberOfDesc;
    // page align is required
    const uintptr_t physicalAddress = syscall_get_physical_address((uintptr_t)writeDesc.data(), NULL);
#if 0
    printf("[virtio] physicalAddress=%x\n", physicalAddress);
#endif
    const uintptr_t alignedAddress = (physicalAddress + PAGE_MASK) & ~PAGE_MASK;
#if 0
    printf("[virtio] alignedAddress=%x\n", alignedAddress);
#endif
    ASSERT((alignedAddress % PAGE_SIZE) == 0);

    // vring.desc is page aligned
    vring.desc = (struct vring_desc*)(writeDesc.data() + alignedAddress - physicalAddress);
    //make linked ring
    for (uintptr_t i = 0; i < vring.num; i++) {
        vring.desc[i].next = i + 1;
        vring.desc[i].len = i + 1; // for debug. With it we can know which desc is wrong state
    }
    vring.desc[vring.num - 1].next = 0;

    // vring.avail is follow after the array of desc
    vring.avail = (struct vring_avail *)&vring.desc[numberOfDesc];
    ASSERT((syscall_get_physical_address((uintptr_t)vring.avail, NULL) % PAGE_SIZE) == 0)


    //vring.used is also page aligned
    const uintptr_t usedPhysicalAddress = syscall_get_physical_address((uintptr_t)&(vring.avail->ring[numberOfDesc]), NULL);
    const uintptr_t usedAligendAddress = (usedPhysicalAddress + PAGE_MASK) & ~PAGE_MASK;
    ASSERT((usedAligendAddress % PAGE_SIZE) == 0);
#if 0
    _printf("usedAligendAddress - usedPhysicalAddress= %d\n", usedAligendAddress - usedPhysicalAddress);
#endif
    vring.used = (struct vring_used*)((uintptr_t)&(vring.avail->ring[numberOfDesc]) + usedAligendAddress - usedPhysicalAddress);

#if 0
    printf("vring.desc=%x vring.used=%x diff=%d\n"
           , syscall_get_physical_address((uintptr_t)vring.desc, NULL)
           , syscall_get_physical_address((uintptr_t)vring.used, NULL)
           , (uintptr_t)syscall_get_physical_address((uintptr_t)vring.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring.desc, NULL));
#endif
    ASSERT((((uintptr_t)syscall_get_physical_address((uintptr_t)vring.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring.desc, NULL))) == 8192);

    ASSERT((uintptr_t)(&(vring.used->ring[numberOfDesc])) <= (uintptr_t)(writeDesc.data() + writeDesc.size()));
    // 9. set up pfn

#if 0
    printf("[virtio] set up PFN\n");
#endif
    outp32(baseAddress + VIRTIO_PCI_QUEUE_PFN, syscall_get_physical_address((uintptr_t)vring.desc, NULL) >> 12);
    //10. prepare the data to write

    Buffer writeData1(PAGE_SIZE * 2);
    const uintptr_t phys1 = syscall_get_physical_address((uintptr_t)writeData1.data(), NULL);
    const uintptr_t aphys1 = (phys1+ PAGE_MASK) & ~PAGE_MASK;
    struct virtio_net_hdr* hdr = (struct virtio_net_hdr*) (writeData1.data() + aphys1 - phys1);
    // This is *necessary*
   hdr->flags = 0;
   hdr->csum_offset = 0;
   hdr->csum_start = 0;
   hdr->gso_type = VIRTIO_NET_HDR_GSO_NONE;
   hdr->gso_size = 0;
   hdr->hdr_len = 0;
    vring.desc[0].flags  |= VRING_DESC_F_NEXT; ; // no next
    vring.desc[0].addr = syscall_get_physical_address((uintptr_t)hdr, NULL);
    vring.desc[0].len = sizeof(struct virtio_net_hdr);

    Buffer writeData2(PAGE_SIZE * 2);

    const uintptr_t phys2 = syscall_get_physical_address((uintptr_t)writeData2.data(), NULL);
    const uintptr_t aphys2 = (phys2+ PAGE_MASK) & ~PAGE_MASK;
    Ether::Frame* frame = (Ether::Frame*) (writeData2.data() + aphys2 - phys2);
    frame->dstmac[0] = 0xff;
    frame->dstmac[1] = 0xff;
    frame->dstmac[2] = 0xff;
    frame->dstmac[3] = 0xff;
    frame->dstmac[4] = 0xff;
    frame->dstmac[5] = 0xff;
    memcpy(frame->srcmac, config.mac, 6);
    frame->type = Util::swapShort(Ether::ARP);
    Arp::Header* header = (Arp::Header*)frame->data;
        header->hardType =Util::swapShort(Arp::HARD_TYPE_ETHER);
        header->protType = Util::swapShort(Arp::PROTCOL_TYPE_IP);
        header->hardAddrLen = 6;
        header->protAddrLen = 4;
        memcpy(header->srcMac, config.mac, 6);
        header->opeCode = Util::swapShort(Arp::OPE_CODE_ARP_REQ);
        header->srcIp = Util::ipAddressToDuint16_t(10, 0, 2, 10);
        // QEMU network is 10.0.2.x
        header->dstIp = Util::ipAddressToDuint16_t(10, 0, 2, 2); // 10.0.2.3 is user mode QEMU DNS
        memset(header->dstMac, 0, 6);
    // 11. set the data to vring
    vring.desc[1].flags = 0; // no next
    vring.desc[1].addr = syscall_get_physical_address((uintptr_t)frame, NULL);
    vring.desc[1].len = sizeof(Ether::Frame);
    vring.avail->idx = 1;
    vring.avail->ring[0] =0;
    // setup receive ring
    // 4. Select the queue to use
    outp16(baseAddress + VIRTIO_PCI_QUEUE_SEL, 0); // 0: read queue, 1: write queue
    // 5. how many descriptors do the queue have?
    const int numberOfDesc2 = inp16(baseAddress + VIRTIO_PCI_QUEUE_NUM);

    printf("[ARP Request] Who has 10.0.2.3? Tell 10.0.2.10\n");

#if 0

    printf("[virtio] numberOfDesc2=%d\n", numberOfDesc2);
#endif
    ASSERT(numberOfDesc2 == 256);

    // 6. Check wheter the queue is already set vring (necessary?).
    uint16_t pfn2 = inp16(baseAddress + VIRTIO_PCI_QUEUE_PFN);

    if (pfn2 != 0) {
        printf("[virtio] pfn2=%x\n", pfn2);
        exit(-1);
    }

    // 7. setup vring
//    uint8_t*  queueData2 = new uint8_t[MAX_QUEUE_SIZE];
//    uint8_t  queueData2[MAX_QUEUE_SIZE];
//    memset(queueData2, 0, MAX_QUEUE_SIZE);

    Buffer readDesc(MAX_QUEUE_SIZE);
    struct vring vring2;
    vring2.num = numberOfDesc2;
    // page align is required
    const uintptr_t physicalAddress2 = syscall_get_physical_address((uintptr_t)readDesc.data(), NULL);
//    printf("[virtio] physicalAddress2=%x\n", physicalAddress2);
    const uintptr_t alignedAddress2 = (physicalAddress2 + PAGE_MASK) & ~PAGE_MASK;
//    printf("[virtio] alignedAddress2=%x\n", alignedAddress2);

    ASSERT((alignedAddress2 % PAGE_SIZE) == 0);

    // vring.desc is page aligned
    vring2.desc = (struct vring_desc*)(readDesc.data() + alignedAddress2 - physicalAddress2);

    // make linked ring
    for (uintptr_t i = 0; i < vring2.num; i++) {
        vring2.desc[i].next = i + 1;
        vring2.desc[i].flags |= VRING_DESC_F_WRITE;
    }
    vring2.desc[vring2.num - 1].next = 0;


    // vring.avail is follow after the array of desc
    vring2.avail = (struct vring_avail *)&vring2.desc[numberOfDesc2];

    // vring.used is also page aligned
    const uintptr_t usedPhysicalAddress2 = syscall_get_physical_address((uintptr_t)&(vring2.avail->ring[numberOfDesc2]), NULL);
    const uintptr_t usedAligendAddress2 = (usedPhysicalAddress2 + PAGE_MASK) & ~PAGE_MASK;
    ASSERT((usedAligendAddress2 % PAGE_SIZE) == 0);
    vring2.used = (struct vring_used*)((uintptr_t)&(vring2.avail->ring[numberOfDesc2]) + usedAligendAddress2 - usedPhysicalAddress2);

//     printf("vring2.desc=%x vring2.used=%x diff=%d\n"
//            , syscall_get_physical_address((uintptr_t)vring2.desc, NULL)
//            , syscall_get_physical_address((uintptr_t)vring2.used, NULL)
//            , (uintptr_t)syscall_get_physical_address((uintptr_t)vring2.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring2.desc, NULL));

    ASSERT((uintptr_t)syscall_get_physical_address((uintptr_t)vring2.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring2.desc, NULL) == 8192);
    ASSERT((((uintptr_t)syscall_get_physical_address((uintptr_t)vring2.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring2.desc, NULL)) % PAGE_SIZE) == 0);

    // 9. set up pfn
//    printf("[virtio] set up PFN\n");
    outp32(baseAddress + VIRTIO_PCI_QUEUE_PFN, syscall_get_physical_address((uintptr_t)vring2.desc, NULL) >> 12);

    // 10. prepare the data to write
//    uint8_t data3[PAGE_SIZE * 2];
    Buffer readData1(PAGE_SIZE * 2);
    const uintptr_t phys3 = syscall_get_physical_address((uintptr_t)readData1.data(), NULL);
    const uintptr_t aphys3 = (phys3+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata = (uint8_t *) (readData1.data() + aphys3 - phys3);

    vring2.desc[0].flags = VRING_DESC_F_NEXT|VRING_DESC_F_WRITE; // no next
    vring2.desc[0].addr = syscall_get_physical_address((uintptr_t)rdata, NULL);
    vring2.desc[0].len = sizeof(struct virtio_net_hdr);

    Buffer readData2(PAGE_SIZE * 2);
    const uintptr_t phys4 = syscall_get_physical_address((uintptr_t)readData2.data(), NULL);
    const uintptr_t aphys4 = (phys4+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata2 = (uint8_t *) (readData2.data() + aphys4 - phys4);
    vring2.desc[1].flags = VRING_DESC_F_WRITE; // no next
    vring2.desc[1].addr = syscall_get_physical_address((uintptr_t)rdata2, NULL);
    vring2.desc[1].len = PAGE_SIZE;
    vring2.avail->idx = 1;
    vring2.avail->ring[0] =0;

    outp16(baseAddress + VIRTIO_PCI_QUEUE_NOTIFY, 0);
    ASSERT(vring.desc[0].flags == VRING_DESC_F_NEXT);
    ASSERT(vring.desc[1].flags == 0);

    ASSERT(vring2.desc[0].flags & VRING_DESC_F_NEXT);
    ASSERT((vring2.desc[1].flags & VRING_DESC_F_NEXT) == 0);
    ASSERT(vring2.desc[1].flags & VRING_DESC_F_WRITE);


    CHECK_BUFFER(writeDesc);
    CHECK_BUFFER(writeData1);
    CHECK_BUFFER(writeData2);
    CHECK_BUFFER(readDesc);
    CHECK_BUFFER(readData1);
    CHECK_BUFFER(readData2);
    // 12. Notify!
    //  printf("[virtio] vring.used->idx = %d \n", vring.used->idx);
    outp16(baseAddress + VIRTIO_PCI_QUEUE_NOTIFY, 1);
    // 13. wait notification from host
    MessageInfo msg;
    Message::receive(&msg);

    switch (msg.header)
    {
    case MSG_INTERRUPTED:
    {
//        printf("[virtio] Interrupt comes\n");
        // clear ISR.
        const uint8_t isr = inp8(baseAddress + VIRTIO_PCI_ISR);
//        printf("[virtio] isr=%x\n", isr);
        monapi_set_irq(pciInf.irqLine, MONAPI_TRUE, MONAPI_TRUE);
        CHECK_BUFFER(writeDesc);
        CHECK_BUFFER(writeData1);
        CHECK_BUFFER(writeData2);
        CHECK_BUFFER(readDesc);
        CHECK_BUFFER(readData1);
        CHECK_BUFFER(readData2);
        break;
    }
    default:
        printf("[virtio] uknown message\n");
        break;
    }

    // if data is written, idx 0 => 1
//     printf("[virtio] vring.used->idx = %d \n", vring.used->idx);

//     printf("***** send done ****");
//     printf("[virtio] vring2.used->idx = %d \n", vring2.used->idx);

//    sleep(1000);

    while (vring2.used->idx == 0) {
        sleep(300);
        printf("waiting \n");
    }
    CHECK_BUFFER(writeDesc);
    CHECK_BUFFER(writeData1);
    CHECK_BUFFER(writeData2);
    CHECK_BUFFER(readDesc);
    CHECK_BUFFER(readData1);
    CHECK_BUFFER(readData2);

    Ether::Frame* rframe = (Ether::Frame*)rdata2;
//    printf("rframe->type=%x\n", rframe->type);

    if (rframe->type == Util::swapShort(Ether::ARP)) {
        Arp::Header* hdr = (Arp::Header*)rframe->data;

        printf("[ARP Reply  ] %s is %d:%d:%d:%d:%d:%d\n"
               , (const char*)Util::ipAddressToCString(hdr->srcIp)
               , hdr->srcMac[0]
               , hdr->srcMac[1]
               , hdr->srcMac[2]
               , hdr->srcMac[3]
               , hdr->srcMac[4]
               , hdr->srcMac[5]);
//         printf("hardType=%x\n", hdr->hardType);
//         printf("protType=%x\n", hdr->protType);
//         printf("hardAddrLen=%x\n", hdr->hardAddrLen);
//         printf("protAddrLen=%x\n", hdr->protAddrLen);
//         printf("opeCode =%x\n", hdr->opeCode );
//         for (int i = 0; i < 6 ; i++) {
//             printf("srcMac=[i]=%x\n", hdr->srcMac[i]);

//         }
//         printf("  srcIp=%x\n",   hdr->srcIp);
//        printf("dstMac[6]=%x\n", dstMac[6]);

    } else {
//        printf("rframe->type=%d \n", rframe->type);
    }

//    printf("[virtio] vring2.used->idx = %d \n", vring2.used->idx);
    {
    MessageInfo msg;
    Message::receive(&msg);

    switch (msg.header)
    {
    case MSG_INTERRUPTED:
    {
        printf("[virtio] Interrupt comes\n");
        // clear ISR.
        const uint8_t isr = inp8(baseAddress + VIRTIO_PCI_ISR);
        printf("[virtio] isr=%x\n", isr);
        monapi_set_irq(pciInf.irqLine, MONAPI_TRUE, MONAPI_TRUE);
        break;
    }
    default:
        printf("[virtio] uknown message\n");
        break;
    }
    }
    printf("[virtio] vring2.used->idx = %d \n", vring2.used->idx);

    CHECK_BUFFER(writeDesc);
    CHECK_BUFFER(writeData1);
    CHECK_BUFFER(writeData2);
    CHECK_BUFFER(readDesc);
    CHECK_BUFFER(readData1);
    CHECK_BUFFER(readData2);

}


#endif

#if 0
// ARP request/reply works
int main(int argc, char* argv[])
{
    // 1. Device probe
    PciInf pciInf;
    Pci pci;
    pci.CheckPciExist(PCI_VENDOR_ID_REDHAT_QUMRANET, PCI_DEVICE_ID_VIRTIO_NET, &pciInf);

    if (!pciInf.isExist) {
        printf("device not found\n");
        exit(-1);
    }

    printf("device found\n");
    printf("baseAdress=%x\n", pciInf.baseAdress);
    printf("irqLine=%x\n", pciInf.irqLine);

    const uintptr_t baseAddress = pciInf.baseAdress & ~1;

        struct virtio_net_config config;
        for (uintptr_t i = 0; i < sizeof(config); i += 4) {
            (((uint32_t*)&config)[i / 4]) = inp32(baseAddress + VIRTIO_PCI_CONFIG + i);
        }

        printf("config.mac= %x:%x:%x:%x:%x:%x\n", config.mac[0], config.mac[1], config.mac[2], config.mac[3], config.mac[4], config.mac[5]);


    // 2. reset the device
    outp8(baseAddress + VIRTIO_PCI_STATUS,  VIRTIO_CONFIG_S_DRIVER | VIRTIO_CONFIG_S_DRIVER_OK); // 0: reset
    printf("[virtio] isr=%x\n", inp8(baseAddress + VIRTIO_PCI_ISR)); // clear ISR.

    // 3. IRQ receiver
    monapi_set_irq(pciInf.irqLine, MONAPI_TRUE, MONAPI_TRUE);
    syscall_set_irq_receiver(pciInf.irqLine, SYS_MASK_INTERRUPT);

    // 4. Select the queue to use
    outp16(baseAddress + VIRTIO_PCI_QUEUE_SEL, 1); // 0: read queue, 1: write queue

    // 5. how many descriptors do the queue have?
    const int numberOfDesc = inp16(baseAddress + VIRTIO_PCI_QUEUE_NUM);
    printf("[virtio] numberOfDesc=%d\n", numberOfDesc);
    ASSERT(numberOfDesc == 256);

    // 6. Check wheter the queue is already set vring (necessary?).
    uint16_t pfn = inp16(baseAddress + VIRTIO_PCI_QUEUE_PFN);
    if (pfn != 0) {
        printf("[virtio] pfn=%x\n", pfn);
        exit(-1);
    }

    // 7. setup vring
    const int MAX_QUEUE_SIZE = PAGE_MASK + vring_size(MAX_QUEUE_NUM);
    printf("[virtio] MAX_QUEUE_SIZE=%d\n", MAX_QUEUE_SIZE);
//    uint8_t* queueData= new uint8_t[MAX_QUEUE_SIZE];
//    uint8_t queueData[MAX_QUEUE_SIZE];
    Buffer writeDesc(MAX_QUEUE_SIZE);
    CHECK_BUFFER(writeDesc);

    struct vring vring;
    vring.num = numberOfDesc;
    // page align is required
    const uintptr_t physicalAddress = syscall_get_physical_address((uintptr_t)writeDesc.data(), NULL);
    printf("[virtio] physicalAddress=%x\n", physicalAddress);
    const uintptr_t alignedAddress = (physicalAddress + PAGE_MASK) & ~PAGE_MASK;
    printf("[virtio] alignedAddress=%x\n", alignedAddress);
    ASSERT((alignedAddress % PAGE_SIZE) == 0);

    // vring.desc is page aligned
    vring.desc = (struct vring_desc*)(writeDesc.data() + alignedAddress - physicalAddress);
    //make linked ring
    for (uintptr_t i = 0; i < vring.num; i++) {
        vring.desc[i].next = i + 1;
        vring.desc[i].len = i + 1; // for debug. With it we can know which desc is wrong state
    }
    vring.desc[vring.num - 1].next = 0;

    // vring.avail is follow after the array of desc
    vring.avail = (struct vring_avail *)&vring.desc[numberOfDesc];
    ASSERT((syscall_get_physical_address((uintptr_t)vring.avail, NULL) % PAGE_SIZE) == 0)


    //vring.used is also page aligned
    const uintptr_t usedPhysicalAddress = syscall_get_physical_address((uintptr_t)&(vring.avail->ring[numberOfDesc]), NULL);
    const uintptr_t usedAligendAddress = (usedPhysicalAddress + PAGE_MASK) & ~PAGE_MASK;
    ASSERT((usedAligendAddress % PAGE_SIZE) == 0);
    _printf("usedAligendAddress - usedPhysicalAddress= %d\n", usedAligendAddress - usedPhysicalAddress);
    vring.used = (struct vring_used*)((uintptr_t)&(vring.avail->ring[numberOfDesc]) + usedAligendAddress - usedPhysicalAddress);

    printf("vring.desc=%x vring.used=%x diff=%d\n"
           , syscall_get_physical_address((uintptr_t)vring.desc, NULL)
           , syscall_get_physical_address((uintptr_t)vring.used, NULL)
           , (uintptr_t)syscall_get_physical_address((uintptr_t)vring.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring.desc, NULL));

    ASSERT((((uintptr_t)syscall_get_physical_address((uintptr_t)vring.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring.desc, NULL))) == 8192);

    ASSERT((uintptr_t)(&(vring.used->ring[numberOfDesc])) <= (uintptr_t)(writeDesc.data() + writeDesc.size()));
    // 9. set up pfn
    printf("[virtio] set up PFN\n");
    outp32(baseAddress + VIRTIO_PCI_QUEUE_PFN, syscall_get_physical_address((uintptr_t)vring.desc, NULL) >> 12);
    //10. prepare the data to write

    Buffer writeData1(PAGE_SIZE * 2);
    const uintptr_t phys1 = syscall_get_physical_address((uintptr_t)writeData1.data(), NULL);
    const uintptr_t aphys1 = (phys1+ PAGE_MASK) & ~PAGE_MASK;
    struct virtio_net_hdr* hdr = (struct virtio_net_hdr*) (writeData1.data() + aphys1 - phys1);
    // This is *necessary*
   hdr->flags = 0;
   hdr->csum_offset = 0;
   hdr->csum_start = 0;
   hdr->gso_type = VIRTIO_NET_HDR_GSO_NONE;
   hdr->gso_size = 0;
   hdr->hdr_len = 0;
    vring.desc[0].flags  |= VRING_DESC_F_NEXT; ; // no next
    vring.desc[0].addr = syscall_get_physical_address((uintptr_t)hdr, NULL);
    vring.desc[0].len = sizeof(struct virtio_net_hdr);

    Buffer writeData2(PAGE_SIZE * 2);

    const uintptr_t phys2 = syscall_get_physical_address((uintptr_t)writeData2.data(), NULL);
    const uintptr_t aphys2 = (phys2+ PAGE_MASK) & ~PAGE_MASK;
    Ether::Frame* frame = (Ether::Frame*) (writeData2.data() + aphys2 - phys2);
    frame->dstmac[0] = 0xff;
    frame->dstmac[1] = 0xff;
    frame->dstmac[2] = 0xff;
    frame->dstmac[3] = 0xff;
    frame->dstmac[4] = 0xff;
    frame->dstmac[5] = 0xff;
    memcpy(frame->srcmac, config.mac, 6);
    frame->type = Util::swapShort(Ether::ARP);
    Arp::Header* header = (Arp::Header*)frame->data;
        header->hardType =Util::swapShort(Arp::HARD_TYPE_ETHER);
        header->protType = Util::swapShort(Arp::PROTCOL_TYPE_IP);
        header->hardAddrLen = 6;
        header->protAddrLen = 4;
        memcpy(header->srcMac, config.mac, 6);
        header->opeCode = Util::swapShort(Arp::OPE_CODE_ARP_REQ);
        header->srcIp = Util::ipAddressToDuint16_t(10, 0, 2, 10);
        // QEMU network is 10.0.2.x
        header->dstIp = Util::ipAddressToDuint16_t(10, 0, 2, 3); // 10.0.2.3 is user mode QEMU DNS
        memset(header->dstMac, 0, 6);
    // 11. set the data to vring
    vring.desc[1].flags = 0; // no next
    vring.desc[1].addr = syscall_get_physical_address((uintptr_t)frame, NULL);
    vring.desc[1].len = sizeof(Ether::Frame);
    vring.avail->idx = 1;
    vring.avail->ring[0] =0;
    // setup receive ring
    // 4. Select the queue to use
    outp16(baseAddress + VIRTIO_PCI_QUEUE_SEL, 0); // 0: read queue, 1: write queue
    // 5. how many descriptors do the queue have?
    const int numberOfDesc2 = inp16(baseAddress + VIRTIO_PCI_QUEUE_NUM);
    printf("[virtio] numberOfDesc2=%d\n", numberOfDesc2);
    ASSERT(numberOfDesc2 == 256);

    // 6. Check wheter the queue is already set vring (necessary?).
    uint16_t pfn2 = inp16(baseAddress + VIRTIO_PCI_QUEUE_PFN);
    if (pfn2 != 0) {
        printf("[virtio] pfn2=%x\n", pfn2);
        exit(-1);
    }

    // 7. setup vring
//    uint8_t*  queueData2 = new uint8_t[MAX_QUEUE_SIZE];
//    uint8_t  queueData2[MAX_QUEUE_SIZE];
//    memset(queueData2, 0, MAX_QUEUE_SIZE);

    Buffer readDesc(MAX_QUEUE_SIZE);
    struct vring vring2;
    vring2.num = numberOfDesc2;
    // page align is required
    const uintptr_t physicalAddress2 = syscall_get_physical_address((uintptr_t)readDesc.data(), NULL);
    printf("[virtio] physicalAddress2=%x\n", physicalAddress2);
    const uintptr_t alignedAddress2 = (physicalAddress2 + PAGE_MASK) & ~PAGE_MASK;
    printf("[virtio] alignedAddress2=%x\n", alignedAddress2);

    ASSERT((alignedAddress2 % PAGE_SIZE) == 0);

    // vring.desc is page aligned
    vring2.desc = (struct vring_desc*)(readDesc.data() + alignedAddress2 - physicalAddress2);

    // make linked ring
    for (uintptr_t i = 0; i < vring2.num; i++) {
        vring2.desc[i].next = i + 1;
        vring2.desc[i].flags |= VRING_DESC_F_WRITE;
    }
    vring2.desc[vring2.num - 1].next = 0;


    // vring.avail is follow after the array of desc
    vring2.avail = (struct vring_avail *)&vring2.desc[numberOfDesc2];

    // vring.used is also page aligned
    const uintptr_t usedPhysicalAddress2 = syscall_get_physical_address((uintptr_t)&(vring2.avail->ring[numberOfDesc2]), NULL);
    const uintptr_t usedAligendAddress2 = (usedPhysicalAddress2 + PAGE_MASK) & ~PAGE_MASK;
    ASSERT((usedAligendAddress2 % PAGE_SIZE) == 0);
    vring2.used = (struct vring_used*)((uintptr_t)&(vring2.avail->ring[numberOfDesc2]) + usedAligendAddress2 - usedPhysicalAddress2);

    printf("vring2.desc=%x vring2.used=%x diff=%d\n"
           , syscall_get_physical_address((uintptr_t)vring2.desc, NULL)
           , syscall_get_physical_address((uintptr_t)vring2.used, NULL)
           , (uintptr_t)syscall_get_physical_address((uintptr_t)vring2.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring2.desc, NULL));

    ASSERT((uintptr_t)syscall_get_physical_address((uintptr_t)vring2.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring2.desc, NULL) == 8192);
    ASSERT((((uintptr_t)syscall_get_physical_address((uintptr_t)vring2.used, NULL) - (uintptr_t)syscall_get_physical_address((uintptr_t)vring2.desc, NULL)) % PAGE_SIZE) == 0);

    // 9. set up pfn
    printf("[virtio] set up PFN\n");
    outp32(baseAddress + VIRTIO_PCI_QUEUE_PFN, syscall_get_physical_address((uintptr_t)vring2.desc, NULL) >> 12);

    // 10. prepare the data to write
//    uint8_t data3[PAGE_SIZE * 2];
    Buffer readData1(PAGE_SIZE * 2);
    const uintptr_t phys3 = syscall_get_physical_address((uintptr_t)readData1.data(), NULL);
    const uintptr_t aphys3 = (phys3+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata = (uint8_t *) (readData1.data() + aphys3 - phys3);

    vring2.desc[0].flags = VRING_DESC_F_NEXT|VRING_DESC_F_WRITE; // no next
    vring2.desc[0].addr = syscall_get_physical_address((uintptr_t)rdata, NULL);
    vring2.desc[0].len = sizeof(struct virtio_net_hdr);

    Buffer readData2(PAGE_SIZE * 2);
    const uintptr_t phys4 = syscall_get_physical_address((uintptr_t)readData2.data(), NULL);
    const uintptr_t aphys4 = (phys4+ PAGE_MASK) & ~PAGE_MASK;
    uint8_t* rdata2 = (uint8_t *) (readData2.data() + aphys4 - phys4);
    vring2.desc[1].flags = VRING_DESC_F_WRITE; // no next
    vring2.desc[1].addr = syscall_get_physical_address((uintptr_t)rdata2, NULL);
    vring2.desc[1].len = PAGE_SIZE;
    vring2.avail->idx = 1;
    vring2.avail->ring[0] =0;

    outp16(baseAddress + VIRTIO_PCI_QUEUE_NOTIFY, 0);
    ASSERT(vring.desc[0].flags == VRING_DESC_F_NEXT);
    ASSERT(vring.desc[1].flags == 0);

    ASSERT(vring2.desc[0].flags & VRING_DESC_F_NEXT);
    ASSERT((vring2.desc[1].flags & VRING_DESC_F_NEXT) == 0);
    ASSERT(vring2.desc[1].flags & VRING_DESC_F_WRITE);


    CHECK_BUFFER(writeDesc);
    CHECK_BUFFER(writeData1);
    CHECK_BUFFER(writeData2);
    CHECK_BUFFER(readDesc);
    CHECK_BUFFER(readData1);
    CHECK_BUFFER(readData2);
    // 12. Notify!
    printf("[virtio] vring.used->idx = %d \n", vring.used->idx);
    outp16(baseAddress + VIRTIO_PCI_QUEUE_NOTIFY, 1);
    // 13. wait notification from host
    MessageInfo msg;
    Message::receive(&msg);

    switch (msg.header)
    {
    case MSG_INTERRUPTED:
    {
        printf("[virtio] Interrupt comes\n");
        // clear ISR.
        const uint8_t isr = inp8(baseAddress + VIRTIO_PCI_ISR);
        printf("[virtio] isr=%x\n", isr);
        monapi_set_irq(pciInf.irqLine, MONAPI_TRUE, MONAPI_TRUE);
        CHECK_BUFFER(writeDesc);
        CHECK_BUFFER(writeData1);
        CHECK_BUFFER(writeData2);
        CHECK_BUFFER(readDesc);
        CHECK_BUFFER(readData1);
        CHECK_BUFFER(readData2);
        break;
    }
    default:
        printf("[virtio] uknown message\n");
        break;
    }

    // if data is written, idx 0 => 1
    printf("[virtio] vring.used->idx = %d \n", vring.used->idx);

    printf("***** send done ****");
    printf("[virtio] vring2.used->idx = %d \n", vring2.used->idx);

//    sleep(1000);

    while (vring2.used->idx == 0) {
        sleep(300);
        printf("waiting \n");
    }
    CHECK_BUFFER(writeDesc);
    CHECK_BUFFER(writeData1);
    CHECK_BUFFER(writeData2);
    CHECK_BUFFER(readDesc);
    CHECK_BUFFER(readData1);
    CHECK_BUFFER(readData2);
    printf("dump***");
    for (int i = 0; i < 1000; i++) {
        if (rdata[i] != 0) {
            printf("[%d]", rdata[i]);
        }
    }

    printf("dump2***");
    for (int i = 0; i < 1000; i++) {
        if (rdata2[i] != 0) {
            printf("[%d]", rdata2[i]);
        }
    }

    Ether::Frame* rframe = (Ether::Frame*)rdata2;
    printf("rframe->type=%x\n", rframe->type);

    if (rframe->type == Util::swapShort(Ether::ARP)) {
        Arp::Header* hdr = (Arp::Header*)rframe->data;
        printf("hardType=%x\n", hdr->hardType);
        printf("protType=%x\n", hdr->protType);
        printf("hardAddrLen=%x\n", hdr->hardAddrLen);
        printf("protAddrLen=%x\n", hdr->protAddrLen);
        printf("opeCode =%x\n", hdr->opeCode );
        for (int i = 0; i < 6 ; i++) {
            printf("srcMac=[i]=%x\n", hdr->srcMac[i]);

        }
        printf("  srcIp=%x\n",   hdr->srcIp);
//        printf("dstMac[6]=%x\n", dstMac[6]);

    } else {
        printf("rframe->type=%d \n", rframe->type);
    }

    printf("[virtio] vring2.used->idx = %d \n", vring2.used->idx);
    {
    MessageInfo msg;
    Message::receive(&msg);

    switch (msg.header)
    {
    case MSG_INTERRUPTED:
    {
        printf("[virtio] Interrupt comes\n");
        // clear ISR.
        const uint8_t isr = inp8(baseAddress + VIRTIO_PCI_ISR);
        printf("[virtio] isr=%x\n", isr);
        monapi_set_irq(pciInf.irqLine, MONAPI_TRUE, MONAPI_TRUE);
        break;
    }
    default:
        printf("[virtio] uknown message\n");
        break;
    }
    }
    printf("[virtio] vring2.used->idx = %d \n", vring2.used->idx);

    CHECK_BUFFER(writeDesc);
    CHECK_BUFFER(writeData1);
    CHECK_BUFFER(writeData2);
    CHECK_BUFFER(readDesc);
    CHECK_BUFFER(readData1);
    CHECK_BUFFER(readData2);

}

#endif

#endif

#if 0

// ARP request is accepted on QEMU.
int main(int argc, char* argv[])
{
    // 1. Device probe
    PciInf pciInf;
    Pci pci;
    pci.CheckPciExist(PCI_VENDOR_ID_REDHAT_QUMRANET, PCI_DEVICE_ID_VIRTIO_NET, &pciInf);

    if (!pciInf.isExist) {
        printf("device not found\n");
        exit(-1);
    }

    printf("device found\n");
    printf("baseAdress=%x\n", pciInf.baseAdress);
    printf("irqLine=%x\n", pciInf.irqLine);

    const uintptr_t baseAddress = pciInf.baseAdress & ~1;

        struct virtio_net_config config;
        for (int i = 0; i < sizeof(config); i += 4) {
            (((uint32_t*)&config)[i / 4]) = inp32(baseAddress + VIRTIO_PCI_CONFIG + i);
        }

        printf("config.mac= %x:%x:%x:%x:%x:%x\n", config.mac[0], config.mac[1], config.mac[2], config.mac[3], config.mac[4], config.mac[5]);


    // 2. reset the device
    outp8(baseAddress + VIRTIO_PCI_STATUS,  VIRTIO_CONFIG_S_DRIVER | VIRTIO_CONFIG_S_DRIVER_OK); // 0: reset
    printf("[virtio] isr=%x\n", inp8(baseAddress + VIRTIO_PCI_ISR)); // clear ISR.

    // 3. IRQ receiver
    monapi_set_irq(pciInf.irqLine, MONAPI_TRUE, MONAPI_TRUE);
    syscall_set_irq_receiver(pciInf.irqLine, SYS_MASK_INTERRUPT);

    // 4. Select the queue to use
    outp16(baseAddress + VIRTIO_PCI_QUEUE_SEL, 1); // 0: read queue, 1: write queue

    // 5. how many descriptors do the queue have?
    const int numberOfDesc = inp16(baseAddress + VIRTIO_PCI_QUEUE_NUM);
    printf("[virtio] numberOfDesc=%d\n", numberOfDesc);
    ASSERT(numberOfDesc > 0);

    // 6. Check wheter the queue is already set vring (necessary?).
    uint16_t pfn = inp16(baseAddress + VIRTIO_PCI_QUEUE_PFN);
    if (pfn != 0) {
        printf("[virtio] pfn=%x\n", pfn);
        exit(-1);
    }

    // 7. setup vring
    const int MAX_QUEUE_SIZE = PAGE_MASK + vring_size(MAX_QUEUE_NUM);
    printf("[virtio] MAX_QUEUE_SIZE=%d\n", MAX_QUEUE_SIZE);
    uint8_t queueData[MAX_QUEUE_SIZE];
    memset(queueData, 0, MAX_QUEUE_SIZE);

    struct vring vring;
    vring.num = numberOfDesc;

    // page align is required
    const uintptr_t physicalAddress = syscall_get_physical_address((uintptr_t)queueData, NULL);
    printf("[virtio] physicalAddress=%x\n", physicalAddress);
    const uintptr_t alignedAddress = (physicalAddress + PAGE_MASK) & ~PAGE_MASK;
    printf("[virtio] alignedAddress=%x\n", alignedAddress);

    ASSERT((alignedAddress % PAGE_SIZE) == 0);

    // vring.desc is page aligned
    vring.desc = (struct vring_desc*)(queueData + alignedAddress - physicalAddress);

    // make linked ring
    for (uintptr_t i = 0; i < vring.num; i++) {
        vring.desc[i].next = i + 1;
    }
    vring.desc[vring.num - 1].next = 0;

    // vring.avail is follow after the array of desc
    vring.avail = (struct vring_avail *)&vring.desc[numberOfDesc];

    // vring.used is also page aligned
    const uintptr_t usedPhysicalAddress = syscall_get_physical_address((uintptr_t)&(vring.avail->ring[numberOfDesc]), NULL);
    const uintptr_t usedAligendAddress = (usedPhysicalAddress + PAGE_MASK) & ~PAGE_MASK;
    ASSERT((usedAligendAddress % PAGE_SIZE) == 0);
    vring.used = (struct vring_used*)((uintptr_t)&(vring.avail->ring[numberOfDesc]) + usedAligendAddress - usedPhysicalAddress);

    // 9. set up pfn
    printf("[virtio] set up PFN\n");
    outp32(baseAddress + VIRTIO_PCI_QUEUE_PFN, syscall_get_physical_address((uintptr_t)vring.desc, NULL) >> 12);

    // 10. prepare the data to write
    uint8_t data1[PAGE_SIZE * 2];
    const uintptr_t phys1 = syscall_get_physical_address((uintptr_t)data1, NULL);
    const uintptr_t aphys1 = (phys1+ PAGE_MASK) & ~PAGE_MASK;
    struct virtio_net_hdr* hdr = (struct virtio_net_hdr*) (data1 + aphys1 - phys1);

    // This is *necessary*
   hdr->flags = 0;
   hdr->csum_offset = 0;
   hdr->csum_start = 0;
   hdr->gso_type = VIRTIO_NET_HDR_GSO_NONE;
   hdr->gso_size = 0;
   hdr->hdr_len = 0;

    vring.desc[0].flags  |= VRING_DESC_F_NEXT; ; // no next
    vring.desc[0].addr = syscall_get_physical_address((uintptr_t)hdr, NULL);
    vring.desc[0].len = sizeof(struct virtio_net_hdr);


    uint8_t data2[PAGE_SIZE * 2];
    const uintptr_t phys2 = syscall_get_physical_address((uintptr_t)data2, NULL);
    const uintptr_t aphys2 = (phys2+ PAGE_MASK) & ~PAGE_MASK;
    Ether::Frame* frame = (Ether::Frame*) (data2 + aphys2 - phys2);
    frame->dstmac[0] = 0xff;
    frame->dstmac[1] = 0xff;
    frame->dstmac[2] = 0xff;
    frame->dstmac[3] = 0xff;
    frame->dstmac[4] = 0xff;
    frame->dstmac[5] = 0xff;

    memcpy(frame->srcmac, config.mac, 6);

    frame->type = Util::swapShort(Ether::ARP);

    Arp::Header* header = (Arp::Header*)frame->data;
        header->hardType =Util::swapShort(Arp::HARD_TYPE_ETHER);
        header->protType = Util::swapShort(Arp::PROTCOL_TYPE_IP);
        header->hardAddrLen = 6;
        header->protAddrLen = 4;
        memcpy(header->srcMac, config.mac, 6);
        header->opeCode = Util::swapShort(Arp::OPE_CODE_ARP_REQ);
        header->srcIp = Util::ipAddressToDuint16_t(10, 0, 2, 10);

        // QEMU network is 10.0.2.x
        header->dstIp = Util::ipAddressToDuint16_t(10, 0, 2, 3); // 10.0.2.3 is user mode QEMU DNS
        memset(header->dstMac, 0, 6);


    // 11. set the data to vring
    vring.desc[1].flags = 0; // no next
    vring.desc[1].addr = syscall_get_physical_address((uintptr_t)frame, NULL);
    vring.desc[1].len = sizeof(Ether::Frame);

    vring.avail->idx = 1;
    vring.avail->ring[0] =0;


    // 12. Notify!
    printf("[virtio] vring.used->idx = %d \n", vring.used->idx);
    outp16(baseAddress + VIRTIO_PCI_QUEUE_NOTIFY, 1);

    // 13. wait notification from host
    MessageInfo msg;
    Message::receive(&msg);

    switch (msg.header)
    {
    case MSG_INTERRUPTED:
    {
        printf("[virtio] Interrupt comes\n");
        // clear ISR.
        const uint8_t isr = inp8(baseAddress + VIRTIO_PCI_ISR);
        printf("[virtio] isr=%x\n", isr);
        monapi_set_irq(pciInf.irqLine, MONAPI_TRUE, MONAPI_TRUE);
        break;
    }
    default:
        printf("[virtio] uknown message\n");
        break;
    }

    // if data is written, idx 0 => 1
    printf("[virtio] vring.used->idx = %d \n", vring.used->idx);

}
#endif
#if 0
// how to use avail
int main(int argc, char* argv[])
{
    // 1. Device probe
    PciInf pciInf;
    Pci pci;
    pci.CheckPciExist(PCI_VENDOR_ID_REDHAT_QUMRANET, PCI_DEVICE_ID_VIRTIO_CONSOLE, &pciInf);

    if (!pciInf.isExist) {
        printf("device not found\n");
        exit(-1);
    }

    printf("device found\n");
    printf("baseAdress=%x\n", pciInf.baseAdress);
    printf("irqLine=%x\n", pciInf.irqLine);

    const uintptr_t baseAddress = pciInf.baseAdress & ~1;

    // 2. reset the device
    outp8(baseAddress + VIRTIO_PCI_STATUS, 0); // 0: reset
    printf("[virtio] isr=%x\n", inp8(baseAddress + VIRTIO_PCI_ISR)); // clear ISR.

    // 3. IRQ receiver
    monapi_set_irq(pciInf.irqLine, MONAPI_TRUE, MONAPI_TRUE);
    syscall_set_irq_receiver(pciInf.irqLine, SYS_MASK_INTERRUPT);

    // 4. Select the queue to use
    outp16(baseAddress + VIRTIO_PCI_QUEUE_SEL, 1); // 0: read queue, 1: write queue

    // 5. how many descriptors do the queue have?
    const int numberOfDesc = inp16(baseAddress + VIRTIO_PCI_QUEUE_NUM);
    printf("[virtio] numberOfDesc=%d\n", numberOfDesc);
    ASSERT(numberOfDesc > 0);

    // 6. Check wheter the queue is already set vring (necessary?).
    uint16_t pfn = inp16(baseAddress + VIRTIO_PCI_QUEUE_PFN);
    if (pfn != 0) {
        printf("[virtio] pfn=%x\n", pfn);
        exit(-1);
    }

    // 7. setup vring
    const int MAX_QUEUE_SIZE = PAGE_MASK + vring_size(MAX_QUEUE_NUM);
    printf("[virtio] MAX_QUEUE_SIZE=%d\n", MAX_QUEUE_SIZE);
    uint8_t queueData[MAX_QUEUE_SIZE];
    memset(queueData, 0, MAX_QUEUE_SIZE);

    struct vring vring;
    vring.num = numberOfDesc;

    // page align is required
    const uintptr_t physicalAddress = syscall_get_physical_address((uintptr_t)queueData, NULL);
    printf("[virtio] physicalAddress=%x\n", physicalAddress);
    const uintptr_t alignedAddress = (physicalAddress + PAGE_MASK) & ~PAGE_MASK;
    printf("[virtio] alignedAddress=%x\n", alignedAddress);

    ASSERT((alignedAddress % PAGE_SIZE) == 0);

    // vring.desc is page aligned
    vring.desc = (struct vring_desc*)(queueData + alignedAddress - physicalAddress);

    // make linked ring
    for (uintptr_t i = 0; i < vring.num; i++) {
        vring.desc[i].next = i + 1;
    }
    vring.desc[vring.num - 1].next = 0;

    // vring.avail is follow after the array of desc
    vring.avail = (struct vring_avail *)&vring.desc[numberOfDesc];

    // vring.used is also page aligned
    const uintptr_t usedPhysicalAddress = syscall_get_physical_address((uintptr_t)&(vring.avail->ring[numberOfDesc]), NULL);
    const uintptr_t usedAligendAddress = (usedPhysicalAddress + PAGE_MASK) & ~PAGE_MASK;
    ASSERT((usedAligendAddress % PAGE_SIZE) == 0);
    vring.used = (struct vring_used*)((uintptr_t)&(vring.avail->ring[numberOfDesc]) + usedAligendAddress - usedPhysicalAddress);

    // 9. set up pfn
    printf("[virtio] set up PFN\n");
    outp32(baseAddress + VIRTIO_PCI_QUEUE_PFN, syscall_get_physical_address((uintptr_t)vring.desc, NULL) >> 12);

    // 10. prepare the data to write
    uint8_t data1[PAGE_SIZE * 2];
    const uintptr_t phys1 = syscall_get_physical_address((uintptr_t)data1, NULL);
    const uintptr_t aphys1 = (phys1+ PAGE_MASK) & ~PAGE_MASK;
    char* p1 = (char*) (data1 + aphys1 - phys1);
    strcpy(p1, "Hello, World");

    // 11. set the data to vring
    vring.desc[0].flags |= VRING_DESC_F_NEXT;
    vring.desc[0].addr = syscall_get_physical_address((uintptr_t)p1, NULL);
    vring.desc[0].len = strlen(p1) + 1;

    uint8_t data2[PAGE_SIZE * 2];
    const uintptr_t phys2 = syscall_get_physical_address((uintptr_t)data2, NULL);
    const uintptr_t aphys2 = (phys2+ PAGE_MASK) & ~PAGE_MASK;
    char* p2 = (char*) (data2 + aphys2 - phys2);
    strcpy(p2, "My name is Higepon");

    // 11. set the data to vring
    vring.desc[1].flags = 0; // no next
    vring.desc[1].addr = syscall_get_physical_address((uintptr_t)p2, NULL);
    vring.desc[1].len = strlen(p2) + 1;

    vring.avail->idx = 1;
    vring.avail->ring[2] =2;


    // 12. Notify!
    printf("[virtio] vring.used->idx = %d \n", vring.used->idx);
    outp16(baseAddress + VIRTIO_PCI_QUEUE_NOTIFY, 1);

    // 13. wait notification from host
    MessageInfo msg;
    Message::receive(&msg);

    switch (msg.header)
    {
    case MSG_INTERRUPTED:
    {
        printf("[virtio] Interrupt comes\n");
        // clear ISR.
        const uint8_t isr = inp8(baseAddress + VIRTIO_PCI_ISR);
        printf("[virtio] isr=%x\n", isr);
        monapi_set_irq(pciInf.irqLine, MONAPI_TRUE, MONAPI_TRUE);
        break;
    }
    default:
        printf("[virtio] uknown message\n");
        break;
    }

    // if data is written, idx 0 => 1
    printf("[virtio] vring.used->idx = %d \n", vring.used->idx);
//     // addr は vring->desc
//     // vring->desc に実際の物理ページのアドレス、長さなどを設定しないとだめ。
//     char* p = new char[4096];
//     memset(p, 0xfe, 4096);

// #define PAGE_SHIFT (12)
// #define PAGE_SIZE  (1<<PAGE_SHIFT)
// #define PAGE_MASK  (PAGE_SIZE-1)

//     p = (char*)(((uint32_t)p + PAGE_MASK) & ~PAGE_MASK);
//     struct vring_desc* desc = (struct vring_desc*)p;

//     ASSERT(((uint32_t)desc % 4096) == 0);

//     char* q = new char[4096];
//     memset(q, 0xfe, 4096);
//     p = (char*)(((uint32_t)q + PAGE_MASK) & ~PAGE_MASK);

//     strcpy(q, "Hello world aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
//     desc->addr = syscall_get_physical_address((uint32_t)q, NULL) >> 12;
//     ASSERT(((uint32_t)q % 4096) == 0);
//     desc->len = strlen(q) + 1;

//     outp32((pciInf.baseAdress & ~1) + VIRTIO_PCI_QUEUE_PFN, syscall_get_physical_address((uint32_t)desc, NULL) >> 12); // sel = 0

// //    outp32((pciInf.baseAdress & ~1) + VIRTIO_PCI_QUEUE_PFN, 0 /* means reset sel = 1 */);
// //    outp32((pciInf.baseAdress & ~1) + VIRTIO_PCI_QUEUE_PFN, (uint32_t)addr >> 12); // sel = 0
//     outp16((pciInf.baseAdress & ~1) + VIRTIO_PCI_QUEUE_NOTIFY, /* sel = 1 */ 1);

    return 0;
}
#endif

#if 0
int main(int argc, char* argv[])
{
    // 1. Device probe
    PciInf pciInf;
    Pci pci;
    pci.CheckPciExist(PCI_VENDOR_ID_REDHAT_QUMRANET, PCI_DEVICE_ID_VIRTIO_CONSOLE, &pciInf);

    if (!pciInf.isExist) {
        printf("device not found\n");
        exit(-1);
    }

    printf("device found\n");
    printf("baseAdress=%x\n", pciInf.baseAdress);
    printf("irqLine=%x\n", pciInf.irqLine);

    const uintptr_t baseAddress = pciInf.baseAdress & ~1;

    // 2. reset the device
    outp8(baseAddress + VIRTIO_PCI_STATUS, 0); // 0: reset
    printf("[virtio] isr=%x\n", inp8(baseAddress + VIRTIO_PCI_ISR)); // clear ISR.

    // 3. IRQ receiver
    monapi_set_irq(pciInf.irqLine, MONAPI_TRUE, MONAPI_TRUE);
    syscall_set_irq_receiver(pciInf.irqLine, SYS_MASK_INTERRUPT);

    // 4. Select the queue to use
    outp16(baseAddress + VIRTIO_PCI_QUEUE_SEL, 1); // 0: read queue, 1: write queue

    // 5. how many descriptors do the queue have?
    const int numberOfDesc = inp16(baseAddress + VIRTIO_PCI_QUEUE_NUM);
    printf("[virtio] numberOfDesc=%d\n", numberOfDesc);
    ASSERT(numberOfDesc > 0);

    // 6. Check wheter the queue is already set vring (necessary?).
    uint16_t pfn = inp16(baseAddress + VIRTIO_PCI_QUEUE_PFN);
    if (pfn != 0) {
        printf("[virtio] pfn=%x\n", pfn);
        exit(-1);
    }

    // 7. setup vring
    const int MAX_QUEUE_SIZE = PAGE_MASK + vring_size(MAX_QUEUE_NUM);
    printf("[virtio] MAX_QUEUE_SIZE=%d\n", MAX_QUEUE_SIZE);
    uint8_t queueData[MAX_QUEUE_SIZE];
    memset(queueData, 0, MAX_QUEUE_SIZE);

    struct vring vring;
    vring.num = numberOfDesc;

    // page align is required
    const uintptr_t physicalAddress = syscall_get_physical_address((uintptr_t)queueData, NULL);
    printf("[virtio] physicalAddress=%x\n", physicalAddress);
    const uintptr_t alignedAddress = (physicalAddress + PAGE_MASK) & ~PAGE_MASK;
    printf("[virtio] alignedAddress=%x\n", alignedAddress);

    ASSERT((alignedAddress % PAGE_SIZE) == 0);

    // vring.desc is page aligned
    vring.desc = (struct vring_desc*)(queueData + alignedAddress - physicalAddress);

    // make linked ring
    for (uintptr_t i = 0; i < vring.num; i++) {
        vring.desc[i].next = i + 1;
    }
    vring.desc[vring.num - 1].next = 0;

    // vring.avail is follow after the array of desc
    vring.avail = (struct vring_avail *)&vring.desc[numberOfDesc];

    // vring.used is also page aligned
    const uintptr_t usedPhysicalAddress = syscall_get_physical_address((uintptr_t)&(vring.avail->ring[numberOfDesc]), NULL);
    const uintptr_t usedAligendAddress = (usedPhysicalAddress + PAGE_MASK) & ~PAGE_MASK;
    ASSERT((usedAligendAddress % PAGE_SIZE) == 0);
    vring.used = (struct vring_used*)((uintptr_t)&(vring.avail->ring[numberOfDesc]) + usedAligendAddress - usedPhysicalAddress);

    // 9. set up pfn
    printf("[virtio] set up PFN\n");
    outp32(baseAddress + VIRTIO_PCI_QUEUE_PFN, syscall_get_physical_address((uintptr_t)vring.desc, NULL) >> 12);

    // 10. prepare the data to write
    uint8_t data[PAGE_SIZE * 2];
    const uintptr_t phys = syscall_get_physical_address((uintptr_t)data, NULL);
    const uintptr_t aphys = (phys+ PAGE_MASK) & ~PAGE_MASK;
    char* p = (char*) (data + aphys - phys);
    strcpy(p, "Hello, World");

    // 11. set the data to vring
    vring.desc[0].flags = 0; // no next
    vring.desc[0].addr = syscall_get_physical_address((uintptr_t)p, NULL);
    vring.desc[0].len = strlen(p) + 1;
    vring.avail->idx = 1;


    // 12. Notify!
    printf("[virtio] vring.used->idx = %d \n", vring.used->idx);
    outp16(baseAddress + VIRTIO_PCI_QUEUE_NOTIFY, 1);

    // 13. wait notification from host
    MessageInfo msg;
    Message::receive(&msg);

    switch (msg.header)
    {
    case MSG_INTERRUPTED:
    {
        printf("[virtio] Interrupt comes\n");
        // clear ISR.
        const uint8_t isr = inp8(baseAddress + VIRTIO_PCI_ISR);
        printf("[virtio] isr=%x\n", isr);
        monapi_set_irq(pciInf.irqLine, MONAPI_TRUE, MONAPI_TRUE);
        break;
    }
    default:
        printf("[virtio] uknown message\n");
        break;
    }

    // if data is written, idx 0 => 1
    printf("[virtio] vring.used->idx = %d \n", vring.used->idx);
//     // addr は vring->desc
//     // vring->desc に実際の物理ページのアドレス、長さなどを設定しないとだめ。
//     char* p = new char[4096];
//     memset(p, 0xfe, 4096);

// #define PAGE_SHIFT (12)
// #define PAGE_SIZE  (1<<PAGE_SHIFT)
// #define PAGE_MASK  (PAGE_SIZE-1)

//     p = (char*)(((uint32_t)p + PAGE_MASK) & ~PAGE_MASK);
//     struct vring_desc* desc = (struct vring_desc*)p;

//     ASSERT(((uint32_t)desc % 4096) == 0);

//     char* q = new char[4096];
//     memset(q, 0xfe, 4096);
//     p = (char*)(((uint32_t)q + PAGE_MASK) & ~PAGE_MASK);

//     strcpy(q, "Hello world aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
//     desc->addr = syscall_get_physical_address((uint32_t)q, NULL) >> 12;
//     ASSERT(((uint32_t)q % 4096) == 0);
//     desc->len = strlen(q) + 1;

//     outp32((pciInf.baseAdress & ~1) + VIRTIO_PCI_QUEUE_PFN, syscall_get_physical_address((uint32_t)desc, NULL) >> 12); // sel = 0

// //    outp32((pciInf.baseAdress & ~1) + VIRTIO_PCI_QUEUE_PFN, 0 /* means reset sel = 1 */);
// //    outp32((pciInf.baseAdress & ~1) + VIRTIO_PCI_QUEUE_PFN, (uint32_t)addr >> 12); // sel = 0
//     outp16((pciInf.baseAdress & ~1) + VIRTIO_PCI_QUEUE_NOTIFY, /* sel = 1 */ 1);

    return 0;
}
#endif


#if 0

struct virtio_net_config
{
    /* The config defining mac address (6 bytes) */
    uint8_t mac[6];
    /* See VIRTIO_NET_F_STATUS and VIRTIO_NET_S_* above */
    uint16_t status;
} __attribute__((packed));

int main(int argc, char* argv[])
{
    PciInf pciInf;
    Pci pci;
    pci.CheckPciExist(PCI_VENDOR_ID_REDHAT_QUMRANET, PCI_DEVICE_ID_VIRTIO_NET, &pciInf);

    if (pciInf.isExist)
    {
        _printf("device found\n");
        _printf("base=%x\n", pciInf.baseAdress);
        _printf("base=%x\n", pciInf.baseAdress & ~1);
        _printf("irqLine=%x\n", pciInf.irqLine);


        _printf("%x", inp32((pciInf.baseAdress & ~1) + VIRTIO_PCI_HOST_FEATURES));

        struct virtio_net_config config;
        for (int i = 0; i < sizeof(config); i += 4) {
            (((uint32_t*)&config)[i / 4]) = inp32((pciInf.baseAdress & ~1) + VIRTIO_PCI_CONFIG + i);
//            (((uint32_t*)&config)[i / 4]) = pci.ReadConfig(0, pciInf.device, 0, (pciInf.baseAdress & ~1) + VIRTIO_PCI_CONFIG + i, 4);
        }

        printf("config.mac= %x:%x:%x:%x:%x:%x\n", config.mac[0], config.mac[1], config.mac[2], config.mac[3], config.mac[4], config.mac[5]);
    } else {
        _printf("device not found\n");
    }

    char* p = new char[4096];
    memset(p, 0xfe, 4096);
    _printf("addr[0] = %x\n", p[0]);

    char* addr = (char*)syscall_get_physical_address((uint32_t)p, 0);

    printf("addr index = %d\n", (uint32_t)addr / 4096);
    // 0 means reset
    outp32((pciInf.baseAdress & ~1) + VIRTIO_PCI_QUEUE_PFN, 0);
    outp32((pciInf.baseAdress & ~1) + VIRTIO_PCI_QUEUE_PFN, (uint32_t)addr / 4096);
    outp32((pciInf.baseAdress & ~1) + VIRTIO_PCI_QUEUE_NOTIFY, 0);

    return 0;
}

#endif
#if 0
int main(int argc, char* argv[])
{
    PciInf pciInf;
    Pci pci;
    pci.CheckPciExist(PCI_VENDOR_ID_REDHAT_QUMRANET, PCI_DEVICE_ID_VIRTIO_CONSOLE, &pciInf);

    if (pciInf.isExist)
    {
        _printf("device found\n");
        _printf("base=%x\n", pciInf.baseAdress & ~1);
        _printf("irqLine=%x\n", pciInf.irqLine);

        for (int i = 0; i < 0xff; i++) {
            outp8(pciInf.baseAdress, i);
        }

    } else {
        _printf("device not found\n");
    }
    return 0;
}
#endif

#endif

#if 0
/*
 * simple.c
 */
//#include <stdio.h>
//#include <string.h>
#include "oniguruma.h"

extern int main(int argc, char* argv[])
{
  int r;
  unsigned char *start, *range, *end;
  regex_t* reg;
  OnigErrorInfo einfo;
  OnigRegion *region;

  char buf[32];
  malloc(30);
  sprintf(buf, "hige");

  static UChar* pattern = (UChar* )"a(.*)b|[e-f]+";
  static UChar* str     = (UChar* )"zzzzaffffffffb";
  _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
  r = onig_new(&reg, pattern, pattern + strlen((char* )pattern),
    ONIG_OPTION_DEFAULT, ONIG_ENCODING_ASCII, ONIG_SYNTAX_DEFAULT, &einfo);
  if (r != ONIG_NORMAL) {
  _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str((uint8_t*)s, r, &einfo);
    fprintf(stderr, "ERROR: %s\n", s);
    return -1;
  }
  _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
  region = onig_region_new();
  _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
  end   = str + strlen((char* )str);
  start = str;
  range = end;
  _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
  r = onig_search(reg, str, end, start, range, region, ONIG_OPTION_NONE);
  _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
  if (r >= 0) {
    int i;
  _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    fprintf(stderr, "match at %d\n", r);
    for (i = 0; i < region->num_regs; i++) {
      fprintf(stderr, "%d: (%d-%d)\n", i, region->beg[i], region->end[i]);
    }
  }
  else if (r == ONIG_MISMATCH) {
    fprintf(stderr, "search fail\n");
  }
  else { /* error */
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str((uint8_t*)s, r);
    fprintf(stderr, "ERROR: %s\n", s);
    return -1;
  }

  onig_region_free(region, 1 /* 1:free self, 0:free contents only */);
  onig_free(reg);
  onig_end();
  return 0;
}

#endif

#if 0

#include <gui/System/Mona/Info.h>

int main(int argc, char* argv[])
{
//    char buf[128];
    printf("Hello, World\n");
    int num;
    uint32_t* handles = System::Mona::gui_enum_windows(&num);
    for (int i = 0; i < num; i++)
    {
        char buffer[WINDOW_TITLE_MAX_LENGTH];
        System::Mona::gui_get_window_title(handles[i], buffer);
        printf("handle=%x %s\n", handles[i], buffer);
    }
    delete[] handles;
//    _printf(buf);
    return -1;
}

#endif

#if 0
extern void dlltest_print();

int main(int argc, char* argv[])
{
    printf("Hello, World \n");
    dlltest_print();
}

#endif
#if 0

static int magic1 = 0xdead;
static int magic2 = 0xbeef;
static int magic3 = 0x1234;
static int magic4 = 0x5678;

extern void dlltest_print();

int main(int argc, char* argv[])
{
    printf("Hello, World \n");
    char buf[26];
    sprintf(buf, "hoge");
    dlltest_print();
//    sprintf(NULL, "hoge");

//     int (*f)(void) = (int (*)(void))0xa0004200;
//     int ret = (*f)();
//     printf("ret=%d\n", ret);
    printf("magic1=%x\n", &magic1);
    printf("magic2=%x\n", &magic2);
    printf("magic3=%x\n", &magic3);
    printf("magic4=%x\n", &magic4);

    return 0;
    for (uint32_t i = 0xA0000000; i < 0xB0000000; i+=2)
    {
        uint16_t* value = (uint16_t*)i;
//        if (i % 100 == 0) printf("%x\n", value);
        if (magic1 == *value || magic2 == *value || magic3 == *value || magic4 == *value)
        {
            printf("found address=%x value=%x\n", value, *value);
        }
    }


    printf("not found\n");
    return 0;
}

#endif

#if 0

int main(int argc, char* argv[])
{

    if (weak_func)
    {
        weak_func();
    }
    else
    {
        printf("weak_func not found\n");
    }
}

#endif

#if 0
int main(int argc, char* argv[]) {

    printf("Hello, World!\n");
    return 0;
}

#endif

#if 0
#include <monalibc/setjmp.h>
jmp_buf g_errorjmp;

int main(int argc, char* argv[]) {

    if(setjmp(g_errorjmp) == 0) {
        printf("JUMP\n");
        longjmp(g_errorjmp, 1);
    }
    printf("END\n");
    return 0;
}
#endif

#if 0
// hz = 1193180 / value;
void beep(uint32_t hz)
{
    uint16_t value = (uint16_t)(1193180 / hz);
    outp8(0x43, 0xb6);
    outp8(0x42, value & 0xff);
    outp8(0x42, value >> 8);
    outp8(0x61, (inp8(0x61) | 0x03) & 0x0f);
}

typedef struct
{
    uint32_t hz;

} Unit;

enum
{
    A = 440,
    B = 467,
    C = 523,
    D = 587,
    E = 659,

};

int main(int argc, char* argv[])
{


    syscall_get_io();

    for (uint16_t i = 440; i < 1200; i++)
    {
        beep(i);
        sleep(10);
    }

    // off
    outp8(0x61, inp8(0x61) & 0x0d);
    printf("end ");

    beep(C);
    sleep(500);
    beep(D);
    sleep(500);
    beep(E);
    sleep(500);
    beep(C);
    sleep(500);
    beep(E);
    sleep(500);
    beep(C);
    sleep(500);
    beep(E);
    sleep(500);
    beep(C);

    return 0;
}
#endif
#if 0
    printf("Hello,World!\n");
#endif

#if 0
    if (argc < 2))
    {
        return -1;
    }
    char* command = argv[1];
    if (strcmp("ECHO", command) == 0)
    {
        char* text = pekoe->get(1);
        monapi_stdout_read((uint8_t*)text, strlen(text) + 1);
    }
    else if (strcmp("UPPER", command) == 0)
    {
        uint8_t buffer[256];
        uint32_t size = monapi_stdin_read(buffer, 256);
        for (uint32_t i = 0; i < size; i++)
        {
            printf("%c", toupper(buffer[i]));
        }
    }
    else
    {
        printf("wrong usage");
    }
#endif

#if 0
    uint32_t fileID = monapi_file_open("/AUTOEXEC.MSH", MONAPI_FALSE);
    monapi_cmemoryinfo* mi = monapi_file_read(fileID, 486);
    printf("AUTOEXEC.MSH size = 485, readResult size = %d\n", mi->Size);
    monapi_file_close(fileID);
    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);
#endif

#if 0
    const char* message = "Hello World\n";
    monapi_cmemoryinfo* buffer = new monapi_cmemoryinfo();
    if (!monapi_cmemoryinfo_create(buffer, strlen(message) + 1, 0))
    {
        monapi_cmemoryinfo_delete(buffer);
        return -1;
    }
    memcpy(buffer->Data, message, buffer->Size);
    uint32_t id = monapi_file_open("/FD/HELLO.TXT", true);
    monapi_file_write(id, buffer, buffer->Size);
    monapi_file_close(id);
    return 0;
}

#endif
