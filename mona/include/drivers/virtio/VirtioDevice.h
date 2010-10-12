/*
 *   Copyright (c) 2010  Higepon(Taro Minowa)  <higepon@users.sourceforge.jp>
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
 */

#ifndef _VIRTIODEVICE_
#define _VIRTIODEVICE_

#include <monapi/Pci.h>
#include <monapi/io.h>
#include <monapi/ContigousMemory.h>
#include <drivers/virtio/VirtBuffer.h>
#include <drivers/virtio/virtio_pci.h>
#include <drivers/virtio/virtio_ring.h>
#include <drivers/virtio/virtio_ids.h>
#include <drivers/virtio/VirtQueue.h>

class VirtioDevice
{
public:
    VirtioDevice(int irq, int basereg) :
        irq_(irq),
        basereg_(basereg)
    {
        // todo
        outp8(basereg_ + VIRTIO_PCI_STATUS,  VIRTIO_CONFIG_S_DRIVER | VIRTIO_CONFIG_S_DRIVER_OK); // 0: reset
    }
    virtual ~VirtioDevice()
    {
    }

    uint8_t getIRQ() const
    {
        return irq_;
    }

    int getBaseReg() const
    {
        return basereg_;
    }

    void enableInterrupt()
    {
        monapi_set_irq(getIRQ(), MONAPI_TRUE, MONAPI_TRUE);
        syscall_set_irq_receiver(getIRQ(), SYS_MASK_INTERRUPT);
    }

    // this waitInterrupt is expensive, so before use this, call delayMicrosec.
    bool waitInterrupt()
    {
        MessageInfo msg;
        uint32_t timerId = set_timer(50);

        for (int i = 0; ; i++)
        {
            int result = MonAPI::Message::peek(&msg, i);

            if (result != M_OK)
            {
                i--;
                syscall_mthread_yield_message();
            }
            else if (msg.header == MSG_TIMER)
            {
                if (msg.arg1 != timerId) continue;
                kill_timer(timerId);

                if (MonAPI::Message::peek(&msg, i, PEEK_REMOVE) != M_OK) {
                    monapi_fatal("peek error %s:%d\n", __FILE__, __LINE__);
                }

//                monapi_warn("interrupt timeout\n");
                return false;
            }
            else if (msg.header == MSG_INTERRUPTED)
            {
                if (msg.arg1 != getIRQ()) continue;
                kill_timer(timerId);

                if (MonAPI::Message::peek(&msg, i, PEEK_REMOVE) != M_OK) {
                    monapi_fatal("peek error %s:%d\n", __FILE__, __LINE__);
                }

                return true;
            }
        }
        return false;
    }

    void getConfig(void* dest, int offset, int sizeByte)
    {
        uint8_t* p = (uint8_t*)dest;
        for (int i = 0; i < sizeByte; i++) {
            p[i] = inp8(basereg_ + VIRTIO_PCI_CONFIG + i + offset);
        }
    }

    uint16_t inp16(uintptr_t addr) const
    {
        return ::inp16(basereg_ + addr);
    }

    void outp16(uintptr_t addr, uint16_t value)
    {
        ::outp16(basereg_ + addr, value);
    }

    uint32_t inp32(uintptr_t addr) const
    {
        return ::inp32(basereg_ + addr);
    }

    void outp32(uintptr_t addr, uint32_t value)
    {
        ::outp32(basereg_ + addr, value);
    }

    uint8_t getStatus() const
    {
        return inp8(basereg_ + VIRTIO_PCI_STATUS);
    }

    void reset()
    {
        outp8(basereg_ + VIRTIO_PCI_STATUS, 0);
    }

    bool hasFeature(int feature) const
    {
        return getFeatures() & (1 << feature);
    }

    VirtQueue* findVirtQueue(int queueIndex)
    {
        return VirtQueue::findVirtQueue(queueIndex, *this);
    }

    static VirtioDevice* probe(int type, unsigned int nth = 0)
    {
        PciInf pciInf;
        Pci pci;
        const int PCI_VENDOR_ID_REDHAT_QUMRANET = 0x1af4;
        pci.CheckPciExist(PCI_VENDOR_ID_REDHAT_QUMRANET, type, nth, &pciInf);

        if (pciInf.isExist) {
            return new VirtioDevice(pciInf.irqLine, pciInf.baseAdress & ~1);
        } else {
            return NULL;
        }
    }


private:
    uint32_t getFeatures() const
    {
        return ::inp32(basereg_ + VIRTIO_PCI_HOST_FEATURES);
    }

    const int irq_;
    const int basereg_;
};


#endif // _VIRTIODEVICE_
