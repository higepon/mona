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

#ifndef MONA_VIRTIO_BLOCK_
#define MONA_VIRTIO_BLOCK_

#include <monapi/io.h>
#include <pci/Pci.h>
#include "virtio.h"
#define VIRTIO_BLK_F_BARRIER    (1 << 0)       /* Does host support barriers? */
#define VIRTIO_BLK_F_SIZE_MAX   (1 << 1)       /* Indicates maximum segment size */
#define VIRTIO_BLK_F_SEG_MAX    (1 << 2)       /* Indicates maximum # of segments */
#define VIRTIO_BLK_F_GEOMETRY   (1 << 4)       /* Indicates support of legacy geometry */
#define VIRTIO_BLK_F_RO         (1 << 5)       /* Disk is read-only */
#define VIRTIO_BLK_F_BLK_SIZE   (1 << 6)       /* Block size of disk is available*/
#define VIRTIO_BLK_F_SCSI       (1 << 7)       /* Supports scsi command passthru */
#define VIRTIO_BLK_F_IDENTIFY   (1 << 8)       /* ATA IDENTIFY supported */
#define VIRTIO_BLK_F_WCACHE     (1 << 9)       /* write cache enabled */

class Virtio
{
public:
    static uint32_t getFeatures(uintptr_t ioaddr)
    {
        return inp32(ioaddr + VIRTIO_PCI_HOST_FEATURES);
    }

};

class VirtioBlock
{
public:
    void test()
    {
        PciInf pciInf;
        Pci pci;
        pci.CheckPciExist(PCI_VENDOR_ID_REDHAT_QUMRANET, PCI_DEVICE_ID_VIRTIO_BLOCK, &pciInf);

        _printf("block is exist %d", pciInf.isExist);
        _printf("baseAdress=%x irqLine=%d", pciInf.baseAdress & ~1, pciInf.irqLine);
        uint32_t feature = Virtio::getFeatures(pciInf.baseAdress & ~1);
        _printf("feature = %x", feature);

        uintptr_t ioaddr = pciInf.baseAdress & ~1;

        _printf("max sector=%d", inp32(ioaddr + VIRTIO_PCI_CONFIG + 0)); // max sector ok
    }
};

#endif // MONA_VIRTIO_BLOCK_
