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

struct etherif {
    etherif()
    {
        lasttime = 0;
        virtioNet = new VirtioNet();
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
