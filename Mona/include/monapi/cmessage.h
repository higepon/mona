#ifndef __MONAPI_CMESSAGE_H__
#define __MONAPI_CMESSAGE_H__

#include <sys/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

extern int monapi_cmessage_send(dword tid, MessageInfo* info);
extern int monapi_cmessage_send_args(dword tid, dword header, dword arg1, dword arg2, dword arg3, const char* str);

extern int monapi_cmessage_receive(MessageInfo* info);
extern int monapi_cmessage_receive_tid(MessageInfo* info, dword tid);
extern int monapi_cmessage_receive_header(MessageInfo* info, dword tid, dword header);
extern int monapi_cmessage_receive_arg1(MessageInfo* info, dword tid, dword header, dword arg1);
extern int monapi_cmessage_receive_header_only(MessageInfo* info, dword header);

extern int monapi_cmessage_send_receive(MessageInfo* result, dword tid, MessageInfo* info);
extern int monapi_cmessage_send_receive_args(MessageInfo* result, dword tid, dword header, dword arg1, dword arg2, dword arg3, const char* str);

extern int monapi_cmessage_reply(MessageInfo* info);
extern int monapi_cmessage_reply_args(MessageInfo* info, dword arg2, dword arg3, const char* str);

extern void monapi_cmessage_create(MessageInfo* info, dword header, dword arg1, dword arg2, dword arg3, const char* str);
extern int monapi_cmessage_exist();

extern dword monapi_cmessage_lookup(const char* name);
extern dword monapi_cmessage_lookup_main_thread(const char* name);

#ifdef __cplusplus
}
#endif

#endif  // __MONAPI_CMESSAGE_H__
