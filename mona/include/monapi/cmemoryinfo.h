#ifndef _MONAPI_CMEMORYINFO_
#define _MONAPI_CMEMORYINFO_

#include <sys/types.h>

typedef struct
{
    uint32_t Handle, Owner, Size;
    uint8_t* Data;
} monapi_cmemoryinfo;

#ifdef __cplusplus
extern "C"
{
#endif
extern monapi_cmemoryinfo* monapi_cmemoryinfo_new();
extern void monapi_cmemoryinfo_delete(monapi_cmemoryinfo*);
extern intptr_t monapi_cmemoryinfo_create(monapi_cmemoryinfo*, uint32_t, int);
extern intptr_t monapi_cmemoryinfo_map(monapi_cmemoryinfo*);
extern void monapi_cmemoryinfo_dispose(monapi_cmemoryinfo*);
extern intptr_t monapi_cmemoryinfo_dispose_no_notify(monapi_cmemoryinfo* self);
#ifdef __cplusplus
}
#endif

#endif  // _MONAPI_CMEMORYINFO_
