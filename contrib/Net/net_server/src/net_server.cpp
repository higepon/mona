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
       char* msg = "Hello!!!!!\n";
       int len = uip_datalen();
       for (int i = 0; i < len; i++)
       {
           _printf("%c", uip_appdata[i]);
       }
       _printf("\n");
       uip_send((uint8_t*)msg, strlen(msg));
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
