#ifndef __PROCESS_SERVER_PROCESS_MANAGER_H__
#define __PROCESS_SERVER_PROCESS_MANAGER_H__

#include "ProcessInfo.h"

extern void initCommonParameters();
extern ProcessInfo getProcessInfo(uint32_t tid);
extern void addProcessInfo(uint32_t tid, uint32_t parent, const MonAPI::CString& path);
extern void addProcessInfo(uint32_t tid, uint32_t parent, const MonAPI::CString& name, const MonAPI::CString& path, uint32_t stdin_id, uint32_t stdout_id);
extern bool processHandler(MessageInfo* msg);

#endif  // __PROCESS_SERVER_PROCESS_MANAGER_H__
