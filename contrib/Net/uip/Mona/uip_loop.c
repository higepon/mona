#include <monalibc.h>
#include <uip.h>
#include <uip_arp.h>

extern unsigned int dev_read();
extern void dev_send();

void uip_log(char *m)
{
    printf("uIP log message: %s\n", m);
}

void uip_loop()
{
    u8_t i, arptimer;


    /* Initialize the HTTP server. */
    httpd_init();

    arptimer = 0;

    while(1) {
        /* Let the monadev network device driver read an entire IP packet
           into the uip_buf. If it must wait for more than 0.5 seconds, it
           will return with the return value 0. If so, we know that it is
           time to call upon the uip_periodic(). Otherwise, the monadev has
           received an IP packet that is to be processed by uIP. */
        uip_len = dev_read();
        if(uip_len == 0) {
            for(i = 0; i < UIP_CONNS; i++) {
                uip_periodic(i);
                /* If the above function invocation resulted in data that
                   should be sent out on the network, the global variable
                   uip_len is set to a value > 0. */
                if(uip_len > 0) {
                    uip_arp_out();
                    dev_send();
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
                    dev_send();
                }
            }
#endif /* UIP_UDP */

            /* Call the ARP timer function every 10 seconds. */
            if(++arptimer == 20) {
                uip_arp_timer();
                arptimer = 0;
            }

        } else {
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])
            if(BUF->type == htons(UIP_ETHTYPE_IP)) {
                uip_arp_ipin();
                uip_input();
                /* If the above function invocation resulted in data that
                   should be sent out on the network, the global variable
                   uip_len is set to a value > 0. */
                if(uip_len > 0) {
                    uip_arp_out();
                    dev_send();
                }
            } else if(BUF->type == htons(UIP_ETHTYPE_ARP)) {
                uip_arp_arpin();
                /* If the above function invocation resulted in data that
                   should be sent out on the network, the global variable
                   uip_len is set to a value > 0. */
                if(uip_len > 0) {
                    dev_send();
                }
            }
        }

    }
    return;
}
