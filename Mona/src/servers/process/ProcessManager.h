#ifndef __PROCESS_SERVER_PROCESS_MANAGER_H__
#define __PROCESS_SERVER_PROCESS_MANAGER_H__

#include "ProcessInfo.h"

extern void initCommonParameters();
extern ProcessInfo getProcessInfo(dword tid);
extern void addProcessInfo(const MonAPI::CString& name);
extern void addProcessInfo(dword tid, dword parent, const MonAPI::CString& path);
extern dword addProcessInfo(dword parent, const MonAPI::CString& name, const MonAPI::CString& path);
extern void addProcessInfo(dword tid, dword parent, const MonAPI::CString& name, const MonAPI::CString& path);
extern void removeProcessInfo(dword tid);
extern void notifyProcessChanged(dword header, dword tid, dword parent = 0, const MonAPI::CString& path = NULL);
extern bool processHandler(MessageInfo* msg);

#endif  // __PROCESS_SERVER_PROCESS_MANAGER_H__
