#include <monapi.h>
#include <limits.h>
#include <string>
#include "monadev.h"
#include "uipLoop.h"
extern "C" {
#include <uip.h>
#include <uip_arp.h>
};

using namespace std;

extern void sendWebGetResult(const std::string& text);

WGetInfo wgetinfo;

void webclient_datahandler(char *data, u16_t len)
{
#if 0
    for (uint16_t i = 0; i < len; i++)
    {
        _printf("%c", data[i]);
    }
#endif
    if (len > 0)
    {
        wgetinfo.content += string(data, len);
    }

    // NULL == data means that a request is successfully done.
    if (NULL == data)
    {
        sendWebGetResult(wgetinfo.content);
    }
}
void webclient_connected()
{
    wgetinfo.content = "";
}

void webclient_timedout()
{
    sendWebGetResult(wgetinfo.content);
}

void webclient_aborted()
{
    _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}
void webclient_closed()
{
    _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

void uipLoop()
{
    u8_t i, arptimer;

    /* Initialize the uIP TCP/IP stack. */
    uip_init();

    webclient_init();

    // now ip address only
    // webclient_get("10.0.2.2", 80, "/");
    arptimer = 0;

    while(1) {
        /* Let the monadev network device driver read an entire IP packet
           into the uip_buf. If it must wait for more than 0.5 seconds, it
           will return with the return value 0. If so, we know that it is
           time to call upon the uip_periodic(). Otherwise, the monadev has
           received an IP packet that is to be processed by uIP. */
        if (wgetinfo.exec)
        {
            webclient_get((char*)wgetinfo.host.data(), wgetinfo.port, (char*)wgetinfo.path.data());
            wgetinfo.exec = false;
        }

        uip_len = monadev_read();
        if(uip_len == 0) {
            for(i = 0; i < UIP_CONNS; i++) {
                uip_periodic(i);
                /* If the above function invocation resulted in data that
                   should be sent out on the network, the global variable
                   uip_len is set to a value > 0. */
                if(uip_len > 0) {
                    uip_arp_out();
                    monadev_send();
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
                    monadev_send();
                }
            }
#endif /* UIP_UDP */

            /* Call the ARP timer function every 10 seconds. */
            if(++arptimer == 20) {
                uip_arp_timer();
                arptimer = 0;
            }

        } else {
            if(BUF->type == htons(UIP_ETHTYPE_IP)) {
                uip_arp_ipin();
                uip_input();
                /* If the above function invocation resulted in data that
                   should be sent out on the network, the global variable
                   uip_len is set to a value > 0. */
                if(uip_len > 0) {
                    uip_arp_out();
                    monadev_send();
                }
            } else if(BUF->type == htons(UIP_ETHTYPE_ARP)) {
                uip_arp_arpin();
                /* If the above function invocation resulted in data that
                   should be sent out on the network, the global variable
                   uip_len is set to a value > 0. */
                if(uip_len > 0) {
                    monadev_send();
                }
            }
        }
    }
}
