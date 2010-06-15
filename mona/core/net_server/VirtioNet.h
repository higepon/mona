/*
 * VirtioNet.h - virtio-net driver
 *
 *   Copyright (c) 2009  Higepon(Taro Minowa)  <higepon@users.sourceforge.jp>
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  $Id: VirtioNet.h 261 2008-07-25 06:16:44Z higepon $
 */

#ifndef MONA_VIRTIO_NET_
#define MONA_VIRTIO_NET_

#include "virtio.h"
#include "IP.h"
#include <monapi/io.h>
#include <pci/Pci.h>

/*
  How to debug the virtio-net driver.

  (1) You can see all in/out packets through tcpdump and wireshark.
      Use command line options shown later.

  (2) You may want to know who does a bad thing that causes a bug.
      virtio-net driver, uIP, QEMU?
      Use logprintf on Mona side and use gettimeofday on QEMU side.

      QEMU: Modify qemu-char.c as follows.
            With this change, every logprintf("\n") call dump a result of gettimeofday.
        int qemu_chr_write(CharDriverState *s, const uint8_t *buf, int len)
        {
          int i;
          struct timeval tm;
          for (i = 0; i < len; i++) {
            if (buf[i] == '\n') {
              gettimeofday(&tm, NULL);
              fprintf(stderr, "(time %lld : %lld)", tm.tv_sec, tm.tv_usec);
            }

            fprintf(stderr, "%c", buf[i]);
          }
            return s->chr_write(s, buf, len);
        }

      And modifing QEMU's virtio-net.c is good idea.
      virtio_net_handle_tx and net.c:ssize_t tap_receive_iov are the place you should check.

  (3) Identifies a invalid packet which causes a bug.
      EtherFrame, ICMP packet and IP packet have identifier, use that.

  (4) QEMU configuration and commandline options.
      % cat /etc/qemu-ifup
      #!/bin/sh
      sudo ifconfig $1 192.168.50.2 up
      sudo rm -f /tmp/tcpdump.log
      sudo tcpdump -i $1 -s 1500 -w /tmp/tcpdump.log &

      % cat /etc/qemu-ifdown
      #!/bin/sh
      sudo ifconfig $1 down
      wireshark /tmp/tcpdump.log &
      sudo pkill -kill tcpdump

      make && sudo pkill wireshark; sudo qemu -clock rtc -serial file:/tmp/qemu_mona.log -virtioconsole stdio -net nic,model=virtio -net tap,if_name=tap0  -cdrom /home/taro/mona/tool/mkimg/mona.iso -fda /home/taro/mona/tool/mkimg/mona.img -boot d 2>packet.log

 */
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
    uint16_t lastUsedIndexRead_;
    uint16_t lastUsedIndexWrite_;
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
        ContigousPhysicalMemory* data = new ContigousPhysicalMemory(PAGE_SIZE * 2);
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
        ContigousPhysicalMemory* readDesc = new ContigousPhysicalMemory(MAX_QUEUE_SIZE);
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
        outp32(baseAddress_ + VIRTIO_PCI_QUEUE_PFN, syscall_get_physical_address((uintptr_t)vring->desc) >> 12);
// Not necessary
//        waitInterrupt();

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
//        waitInterrupt();
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
        monapi_set_irq(irqLine_, MONAPI_TRUE, MONAPI_TRUE);
        syscall_set_irq_receiver(irqLine_, SYS_MASK_INTERRUPT);
        return DEVICE_FOUND;
    }

    // todo refactoring, merge with send
    bool send(const char* buf, int len)
    {
        while (lastUsedIndexWrite_ != writeVring_->used->idx) {
            // Wait for last send is done.
            // In almost case, we expect last send is already done.
            VIRT_LOG("Waiting previous packet is sent");
        }
        Ether::Frame* rframe = (Ether::Frame*)buf;
        memset(writeFrame_, 0, sizeof(Ether::Frame));
        memcpy(writeFrame_, buf, len);
        writeVring_->desc[1].len = len; // todo
        writeVring_->avail->ring[writeVring_->avail->idx % writeVring_->num] = 0; // desc[0] -> desc[1] is used for buffer
        writeVring_->avail->idx++;
        // Before notify, save the lastUsedIndexWrite_
        lastUsedIndexWrite_ = writeVring_->used->idx + 1;
        outp16(baseAddress_ + VIRTIO_PCI_QUEUE_NOTIFY, VRING_TYPE_WRITE);
        return true;
    }

    // N.B.
    // Since it is hard to distiguish sent and received interruption, we don't use interruption for notification of packet send.
    // Instread, we check whether last send is done before sending packet.
    bool send(Ether::Frame* src, int len)
    {
        while (lastUsedIndexWrite_ != writeVring_->used->idx) {
            // Wait for last send is done.
            // In almost case, we expect last send is already done.
            VIRT_LOG("Waiting previous packet is sent");
        }
        memset(writeFrame_, 0, sizeof(Ether::Frame));
        memcpy(writeFrame_, src, len);
        writeVring_->desc[1].len = len; // todo
        writeVring_->avail->ring[writeVring_->avail->idx % writeVring_->num] = 0; // desc[0] -> desc[1] is used for buffer

        writeVring_->avail->idx++;

        // Before notify, save the lastUsedIndexWrite_
        lastUsedIndexWrite_ = writeVring_->used->idx + 1;
        outp16(baseAddress_ + VIRTIO_PCI_QUEUE_NOTIFY, VRING_TYPE_WRITE);
        return true;
    }

    bool receive(Ether::Frame* dst, unsigned int* len, int timeout_msec = 20)
    {
        // Wait a packet coming timeout_msec.
        MessageInfo msg;
        for (int i = 0; ; i++) {
            //  We my lose some interruptions for a packet comming.
            //  So check here.
            if (readVring_->used->idx != lastUsedIndexRead_) {
                break;
            }
            if (timeout_msec < 0) {
                // timeout
                return false;
            }
            intptr_t result = MonAPI::Message::peek(&msg, i);
            // Message not found
            if (result == M_BAD_INDEX) {
                i--;
                sleep(10);
                timeout_msec -= 10;
                continue;
            } else if (result == M_OK) {
                if (msg.header == MSG_INTERRUPTED) {
                    MonAPI::Message::peek(&msg, i, PEEK_REMOVE);
                    inp8(baseAddress_ + VIRTIO_PCI_ISR);
                    monapi_set_irq(irqLine_, MONAPI_TRUE, MONAPI_TRUE);

                    if (readVring_->used->idx == lastUsedIndexRead_) {
                        sleep(10);
                        timeout_msec -= 10;
                        continue;
                    } else {
                        break;
                    }
                } else {
                    continue;
                }
            } else {
                _printf("oooooooooooooooooooooo");
                ASSERT(false);
            }
        }

        const int index = lastUsedIndexRead_ % readVring_->num;
        *len = readVring_->used->ring[index].len - sizeof(struct virtio_net_hdr);
        uint32_t id = readVring_->used->ring[index].id;

        // assume ring size = 5
        ASSERT(id == 0 || id == 2 || id == 4 || id == 6 || id == 8);
        Ether::Frame* rframe = readFrames_[id / 2];
        memcpy(dst, rframe, *len);

        // current used buffer is no more necessary, give it back to tail of avail->ring
        readVring_->avail->ring[readVring_->avail->idx % readVring_->num] = id;
        // increment avail->idx, we should not take remainder of avail->idx ?
        readVring_->avail->idx++;
//    readVring_->avail->idx = (readVring_->avail->idx + 1) % readVring_->num;
        lastUsedIndexRead_++;

        if (!(readVring_->used->flags & VRING_USED_F_NO_NOTIFY)) {
            VIRT_LOG("NOTIFY");
            outp16(baseAddress_ + VIRTIO_PCI_QUEUE_NOTIFY, 0);
        }
        return true;
    }

    bool receive(char* dst, unsigned int* len, int timeout_msec = 20)
    {
        // Wait a packet coming timeout_msec.
        MessageInfo msg;
        for (int i = 0; ; i++) {
            //  We my lose some interruptions for a packet comming.
            //  So check here.
            if (readVring_->used->idx != lastUsedIndexRead_) {
                break;
            }
            if (timeout_msec < 0) {
                // timeout
                return false;
            }
            int result = MonAPI::Message::peek(&msg, i);
            // Message not found
            if (result == M_BAD_INDEX) {
                i--;
                sleep(10);
                timeout_msec -= 10;
                continue;
            } else if (result == M_OK) {
                if (msg.header == MSG_INTERRUPTED) {
                    MonAPI::Message::peek(&msg, i, PEEK_REMOVE);
                    inp8(baseAddress_ + VIRTIO_PCI_ISR);
                    monapi_set_irq(irqLine_, MONAPI_TRUE, MONAPI_TRUE);

                    if (readVring_->used->idx == lastUsedIndexRead_) {
                        sleep(10);
                        timeout_msec -= 10;
                        continue;
                    } else {
                        break;
                    }
                } else {
                    continue;
                }
            } else {
                _printf("peek error");
                ASSERT(false);
            }
        }

        const int index = lastUsedIndexRead_ % readVring_->num;
        *len = readVring_->used->ring[index].len - sizeof(struct virtio_net_hdr);
        uint32_t id = readVring_->used->ring[index].id;
        // assume ring size = 5
        ASSERT(id == 0 || id == 2 || id == 4 || id == 6 || id == 8);
        Ether::Frame* rframe = readFrames_[id / 2];
        memcpy(dst, rframe, *len);

//        IP::Header* ipHeader = (IP::Header*)(rframe->data);
//        logprintf("receive:ip packet %d %x %d\n", ipHeader->tos, ipHeader->srcip, ipHeader->len);
//         for (int i = 0; i < *len; i++) {
//             _printf("[%c]", dst[i]);
//         }

        // current used buffer is no more necessary, give it back to tail of avail->ring
        readVring_->avail->ring[readVring_->avail->idx % readVring_->num] = id;
        // increment avail->idx, we should not take remainder of avail->idx ?
//        _logprintf("%s:%d %d\n", __FILE__, __LINE__, readVring_->avail->idx);
        readVring_->avail->idx++;
//    readVring_->avail->idx = (readVring_->avail->idx + 1) % readVring_->num;
//    _logprintf("%s:%d\n", __FILE__, __LINE__);
        lastUsedIndexRead_++;
        if (!(readVring_->used->flags & VRING_USED_F_NO_NOTIFY)) {
            VIRT_LOG("NOTIFY");
            outp16(baseAddress_ + VIRTIO_PCI_QUEUE_NOTIFY, 0);
        }
        return true;
    }

};


#endif // MONA_VIRTIO_NET_
