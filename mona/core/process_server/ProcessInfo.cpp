#include "ProcessInfo.h"
#include "monapi.h"

using namespace MonAPI;

ProcessInfo::ProcessInfo(uint32_t tid) : tid(THREAD_UNKNOWN), parent(THREAD_UNKNOWN), stdout_id(THREAD_UNKNOWN)
{
//     PsInfo psInfo;
//     syscall_set_ps_dump();
//     while (syscall_read_ps_dump(&psInfo) == 0)
//     {
//         if (psInfo.tid == tid) break;
//     }
//     _printf("%s You can't use printf, use _printf instead.\n", psInfo.name);
//     _printf("Because you process is executed from monitor server, so you have no stdout\n at %s %s:%d\n", __func__, __FILE__, __LINE__);
}

ProcessInfo::ProcessInfo(const ProcessInfo& pi)
    : tid(pi.tid), parent(pi.parent), name(pi.name), path(pi.path), stdin_id(pi.stdin_id), stdout_id(pi.stdout_id)
{
}

ProcessInfo::ProcessInfo(uint32_t tid, uint32_t parent, const CString& name, const CString& path, uint32_t stdin_id, uint32_t stdout_id)
    : tid(tid), parent(parent), name(name), path(path), stdin_id(stdin_id), stdout_id(stdout_id)
{
} 

ProcessInfo& ProcessInfo::operator =(const ProcessInfo& pi)
{
    this->tid       = pi.tid;
    this->parent    = pi.parent;
    this->name      = pi.name;
    this->path      = pi.path;
    this->stdin_id  = pi.stdin_id;
    this->stdout_id = pi.stdout_id;
    return *this;
}
