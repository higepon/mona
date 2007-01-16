#ifndef __PROCESS_SERVER_PROCESS_MANAGER_H__
#define __PROCESS_SERVER_PROCESS_MANAGER_H__

#include "ProcessInfo.h"

extern void initCommonParameters();
extern ProcessInfo getProcessInfo(uint32_t tid);
extern void addProcessInfo(const MonAPI::CString& name);
extern void addProcessInfo(uint32_t tid, uint32_t parent, const MonAPI::CString& path);
extern uint32_t addProcessInfo(uint32_t parent, const MonAPI::CString& name, const MonAPI::CString& path, uint32_t stdin_id, uint32_t stdout_id);
extern void addProcessInfo(uint32_t tid, uint32_t parent, const MonAPI::CString& name, const MonAPI::CString& path, uint32_t stdin_id, uint32_t stdout_id);
extern void removeProcessInfo(uint32_t tid);
extern void notifyProcessChanged(uint32_t header, uint32_t tid, uint32_t parent = 0, const MonAPI::CString& path = NULL);
extern bool processHandler(MessageInfo* msg);
extern void registerStdout(uint32_t tid, uint32_t standardout);

#endif  // __PROCESS_SERVER_PROCESS_MANAGER_H__
