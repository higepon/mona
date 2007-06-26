#include "net_server.h"
extern "C"
{
#include <uip.h>
#include <uip_arp.h>
}

#include <monapi.h>

void net_server_init()
{
  uip_listen(HTONS(5555));
}

void net_server_appcall()
{
   if(uip_newdata() || uip_rexmit())
   {
       _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
       char* msg = "OK\n";
       uip_send((uint8_t*)msg, 3);
   }
}
