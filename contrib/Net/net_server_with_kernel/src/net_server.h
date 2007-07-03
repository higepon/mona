#ifndef __NET_SERVER_H__
#define __NET_SERVER_H__

#include <stdint.h>

typedef struct net_server_state
{
    uint8_t state;
} net_server_state;

#ifndef UIP_APPCALL
#define UIP_APPCALL net_server_appcall
#endif

#ifndef UIP_APPSTATE_SIZE
#define UIP_APPSTATE_SIZE (sizeof(net_server_state))
#endif

#ifdef __cplusplus
extern "C"
{
#endif

void net_server_init();
void net_server_appcall();

#ifdef __cplusplus
}
#endif


#endif /* __NET_SERVER_H__ */
