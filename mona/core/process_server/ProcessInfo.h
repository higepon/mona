#ifndef __PROCESS_SERVER_PROCESS_INFO_H__
#define __PROCESS_SERVER_PROCESS_INFO_H__

#include <sys/types.h>
#include <monapi/CString.h>

struct ProcessInfo
{
    dword tid, parent;
    MonAPI::CString name, path;

    ProcessInfo(int dummy = NULL);
    ProcessInfo(const ProcessInfo& pi);
    ProcessInfo(dword tid, dword parent, const MonAPI::CString& name, const MonAPI::CString& path);
    inline bool operator ==(const ProcessInfo& pi) { return this->tid == pi.tid; }
    inline bool operator !=(const ProcessInfo& pi) { return this->tid != pi.tid; }
    ProcessInfo& operator =(const ProcessInfo& pi);
};

#endif  // __PROCESS_SERVER_PROCESS_INFO_H__
