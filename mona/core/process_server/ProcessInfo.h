#ifndef __PROCESS_SERVER_PROCESS_INFO_H__
#define __PROCESS_SERVER_PROCESS_INFO_H__

#include <sys/types.h>
#include <monapi/CString.h>

struct ProcessInfo
{
    uint32_t tid, parent;
    MonAPI::CString name, path;
    uint32_t stdin_id, stdout_id;

    ProcessInfo(uint32_t tid);
    ProcessInfo(const ProcessInfo& pi);
    ProcessInfo(uint32_t tid, uint32_t parent, const MonAPI::CString& name, const MonAPI::CString& path, uint32_t stdin_id, uint32_t stdout_id);
    inline bool operator ==(const ProcessInfo& pi) { return this->tid == pi.tid; }
    inline bool operator !=(const ProcessInfo& pi) { return this->tid != pi.tid; }
    ProcessInfo& operator =(const ProcessInfo& pi);
};

#endif  // __PROCESS_SERVER_PROCESS_INFO_H__
