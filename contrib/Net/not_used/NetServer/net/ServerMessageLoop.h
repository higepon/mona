#ifndef __SERVER_H__
#define __SERVER_H__

#include <sys/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

void server_init(void);
void server_appcall(void);

#ifndef UIP_APPCALL
#define UIP_APPCALL     server_appcall
#endif

#pragma pack(1)
struct server_state {
    u8_t state;
    char* message;
    uint32_t length;
};
#pragma pack(0)

#ifndef UIP_APPSTATE_SIZE
#define UIP_APPSTATE_SIZE (sizeof(struct server_state))
#endif

#if defined(__cplusplus)
}
#endif

#endif /* __SERVER_H__ */
