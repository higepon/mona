/*
 * etherif.cpp -
 *
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
 *  $Id: etherif.cpp 183 2008-07-04 06:19:28Z higepon $
 */

#include "etherif.h"
#include <monapi.h>
#include "ContigousPhysicalMemory.h"
#include "Ether.h"
#include "VirtioNet.h"

#define IFNAME0 'e'
#define IFNAME1 't'

uintptr_t ContigousPhysicalMemory::startAddress = 0x9E000000;

struct etherif {
    etherif()
    {
        lasttime = 0;
        virtioNet = new VirtioNet();

        const int numberOfReadBufferes = 5;
        enum VirtioNet::DeviceState state = virtioNet->probe(numberOfReadBufferes);
        if (state != VirtioNet::DEVICE_FOUND) {
            _printf("[virtio] virtio-net device not found\n");
            exit(-1);
        }
// qemu -net user mode:
//   we send DHCP request to QEMU and get an ip address.
#ifdef USE_QEMU_USER_NETWORK
    DHCPClient dhcp(virtioNet, virtioNet->macAddress());;
    uint32_t hostAddress = 0;
    uint32_t gatewayAddress = 0;
    if (!dhcp.request(hostAddress, gatewayAddress)) {
        _printf("[uIP] DHCP server not found. exit server\n");
        exit(-1);
    }
#endif
    }

    ~etherif()
    {
        delete virtioNet;
    }

    uint32_t lasttime;
    VirtioNet* virtioNet;
};


/*-----------------------------------------------------------------------------------*/
/*
 * low_level_output():
 *
 * Should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 */
/*-----------------------------------------------------------------------------------*/
static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    char buf[1514];
    char* bufptr;

    struct etherif* etherif = (struct etherif*)netif->state;

    /* initiate transfer(); */

    bufptr = &buf[0];

    for(struct pbuf* q = p; q != NULL; q = q->next) {
        /* Send the data from the pbuf to the interface, one pbuf at a
           time. The size of the data in each pbuf is kept in the ->len
           variable. */
        /* send data from(q->payload, q->len); */
        memcpy(bufptr, q->payload, q->len);
        bufptr += q->len;
    }

    if (!etherif->virtioNet->send(buf, p->tot_len)) {
        fprintf(stderr, "virtionet error: %s:%d\n", __FILE__, __LINE__);
        exit(-1);
    }

    /* signal that packet should be sent(); */
    // snmp_add_ifoutoctets(netif, written);
    return ERR_OK;
}


err_t etherif_init(struct netif* netif)
{
    struct etherif* etherif = new struct etherif;
    netif->state = etherif;
#if LWIP_SNMP
    /* ifType is other(1), there doesn't seem
       to be a proper type for the tunnel if */
    netif->link_type = 1;
    /* @todo get this from struct tunif? */
    netif->link_speed = 0;
    netif->ts = 0;
    netif->ifinoctets = 0;
    netif->ifinucastpkts = 0;
    netif->ifinnucastpkts = 0;
    netif->ifindiscards = 0;
    netif->ifoutoctets = 0;
    netif->ifoutucastpkts = 0;
    netif->ifoutnucastpkts = 0;
    netif->ifoutdiscards = 0;
#endif

    netif->hwaddr_len = 6;
    memcpy(netif->hwaddr, etherif->virtioNet->macAddress(), netif->hwaddr_len);
    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
    netif->output = etharp_output;
    netif->linkoutput = low_level_output;
    netif->mtu = 1500;
    return ERR_OK;
}

static struct pbuf* low_level_input(struct netif* netif)
{
    struct pbuf *p, *q;
    char buf[1514];
    char *bufptr;
    struct etherif* etherif;

    etherif = (struct etherif*)netif->state;

    /* Obtain the size of the packet and put it into the "len"
       variable. */
    ASSERT(sizeof(buf) > sizeof(Ether::Frame));
    unsigned int readSzie = 0;

    // todo timeout
    if (!etherif->virtioNet->receive(buf, &readSzie)) {
        printf("virtio receive timeout");
        return NULL;
    }
//    snmp_add_ifinoctets(netif, readSzie);

    /* We allocate a pbuf chain of pbufs from the pool. */
    p = pbuf_alloc(PBUF_LINK, readSzie, PBUF_POOL);

    if (p != NULL) {
        /* We iterate over the pbuf chain until we have read the entire
           packet into the pbuf. */
        bufptr = &buf[0];
        for(q = p; q != NULL; q = q->next) {
            /* Read enough bytes to fill this pbuf in the chain. The
               available data in the pbuf is given by the q->len
               variable. */
            /* read data into(q->payload, q->len); */
            memcpy(q->payload, bufptr, q->len);
            bufptr += q->len;
        }
        /* acknowledge that packet has been read(); */
    } else {
        /* drop packet(); */
//        snmp_inc_ifindiscards(netif);
        printf("Could not allocate pbufs\n");
    }

    return p;
}

/*-----------------------------------------------------------------------------------*/
/*
 * mintapif_input():
 *
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface.
 *
 */
/*-----------------------------------------------------------------------------------*/
static bool etherif_input(struct netif *netif)
{
    struct etherif* etherif = (struct etherif*)netif->state;
    struct eth_hdr *ethhdr;
    struct pbuf *p;

    p = low_level_input(netif);

    if (p != NULL) {

#if LINK_STATS
        lwip_stats.link.recv++;
#endif /* LINK_STATS */

        ethhdr = (struct eth_hdr*)p->payload;

        switch (htons(ethhdr->type)) {
        case ETHTYPE_IP:
#if 0
/* CSi disabled ARP table update on ingress IP packets.
   This seems to work but needs thorough testing. */
            etharp_ip_input(netif, p);
#endif
            pbuf_header(p, -14);
            netif->input(p, netif);
            break;
        case ETHTYPE_ARP:
            struct eth_addr ethaddr;
            memcpy(ethaddr.addr, etherif->virtioNet->macAddress(), ETHARP_HWADDR_LEN);
            etharp_arp_input(netif, &ethaddr, p);
            break;
        default:
            LWIP_ASSERT("p != NULL", p != NULL);
            pbuf_free(p);
            break;
        }
        return true;
    } else {
        return false;
    }
}


// int
// mintapif_select(struct netif *netif)
// {
//   fd_set fdset;
//   int ret;
//   struct timeval tv;
//   struct mintapif *mintapif;

//   mintapif = netif->state;

//   tv.tv_sec = 0;
//   tv.tv_usec = 0; /* usec_to; */
  
//   FD_ZERO(&fdset);
//   FD_SET(mintapif->fd, &fdset);

//   ret = select(mintapif->fd + 1, &fdset, NULL, NULL, &tv);
//   if (ret > 0) {
//     mintapif_input(netif);   
//   }
//   return ret;
// }

// returns a positive number if data is read.
int etherif_select(struct netif *netif)
{
    if (etherif_input(netif)) {
        return 1;
    } else {
        return 0;
    }
}
