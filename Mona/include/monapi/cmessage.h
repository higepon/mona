#ifndef __MONAPI_CMESSAGE_H__
#define __MONAPI_CMESSAGE_H__

#include <sys/types.h>
#include <monapi/clist.h>

#define MONAPI_BOOL  int
#define MONAPI_TRUE  1
#define MONAPI_FALSE 0

#ifdef __cplusplus
extern "C"
{
#endif

extern int monapi_cmessage_send(dword tid, MessageInfo* info);
extern int monapi_cmessage_send_args(dword tid, dword header, dword arg1, dword arg2, dword arg3, const char* str);

extern int monapi_cmessage_receive(monapi_clist* queue, MessageInfo* dst);
extern int monapi_cmessage_receive_cond(monapi_clist* queue, MessageInfo* dst, MessageInfo* src, MONAPI_BOOL(*cond)(MessageInfo*, MessageInfo*));

extern int monapi_cmessage_send_receive(monapi_clist* queue, MessageInfo* dst, dword tid, MessageInfo* info);
extern int monapi_cmessage_send_receive_args(monapi_clist* queue, MessageInfo* dst, dword tid, dword header, dword arg1, dword arg2, dword arg3, const char* str);

extern int monapi_cmessage_reply(MessageInfo* info);
extern int monapi_cmessage_reply_args(MessageInfo* info, dword arg2, dword arg3, const char* str);

extern void monapi_cmessage_create(MessageInfo* info, dword header, dword arg1, dword arg2, dword arg3, const char* str);
extern int monapi_cmessage_exist(monapi_clist* queue);

extern dword monapi_cmessage_lookup(const char* name);
extern dword monapi_cmessage_lookup_main_thread(const char* name);

extern MONAPI_BOOL monapi_cmessage_cond_from(MessageInfo* msg1, MessageInfo* msg2);
extern MONAPI_BOOL monapi_cmessage_cond_from_header(MessageInfo* msg1, MessageInfo* msg2);
extern MONAPI_BOOL monapi_cmessage_cond_from_header_arg1(MessageInfo* msg1, MessageInfo* msg2);
extern MONAPI_BOOL monapi_cmessage_cond_header(MessageInfo* msg1, MessageInfo* msg2);

#ifdef __cplusplus
}
#endif

#endif  /* __MONAPI_CMESSAGE_H__ */
