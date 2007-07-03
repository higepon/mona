#include "net_server.h"
extern "C"
{
#include <uip.h>
#include <uip_arp.h>
}
#include <stdio.h>
#include <monapi.h>

void net_server_init()
{
    uip_listen(HTONS(5555));
}

void net_server_appcall()
{
   if(uip_newdata() || uip_rexmit())
   {
       char buf1[64];
       char buf2[256];
       memcpy(buf1, (void*)uip_appdata, uip_datalen());
       buf1[uip_datalen()] = '\0';
       sprintf(buf2, "response to %s\n", buf1);
       _printf(buf2);
       uip_send((uint8_t*)buf2, strlen(buf2));
   }

  if(uip_aborted())
  {
      _printf("aborted\n");
  }
  if(uip_timedout())
  {
      _printf("timeout\n");
  }
  if(uip_closed())
  {
      _printf("closed\n");
  }
  if(uip_connected())
  {
      _printf("connected\n");
  }
  if(uip_acked())
  {
      _printf("acked\n");
  }
}
