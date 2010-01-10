/*
 * virtio.h - virtio definitions.
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
 *  $Id: virtio.h 261 2008-07-25 06:16:44Z higepon $
 */

#ifndef VIRTIO_
#define VIRTIO_

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


struct virtio_net_config
{
    /* The config defining mac address (6 bytes) */
    uint8_t mac[6];
    /* See VIRTIO_NET_F_STATUS and VIRTIO_NET_S_* above */
    uint16_t status;
} __attribute__((packed));

#define VIRT_LOG(...) //_logprintf("[%s:%d] ", __FILE__, __LINE__), _logprintf(__VA_ARGS__), _logprintf("\n")

#endif // VIRTIO_
