#ifndef __MONAPI_CMEMORYINFO_H__
#define __MONAPI_CMEMORYINFO_H__

#include <sys/types.h>

typedef struct
{
    dword Handle, Owner, Size;
    byte* Data;
} monapi_cmemoryinfo;

#ifdef __cplusplus
extern "C"
{
#endif
extern monapi_cmemoryinfo* monapi_cmemoryinfo_new();
extern void monapi_cmemoryinfo_delete(monapi_cmemoryinfo*);
extern int monapi_cmemoryinfo_create(monapi_cmemoryinfo*, dword, int);
extern int monapi_cmemoryinfo_map(monapi_cmemoryinfo*);
extern void monapi_cmemoryinfo_dispose(monapi_cmemoryinfo*);
#ifdef __cplusplus
}
#endif

#endif  // __MONAPI_CMEMORYINFO_H__
