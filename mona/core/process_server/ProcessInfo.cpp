#include "ProcessInfo.h"
#include "monapi.h"

using namespace MonAPI;

ProcessInfo::ProcessInfo() :
    tid(THREAD_UNKNOWN),
    parent(THREAD_UNKNOWN),
    name("unknown"),
    path("unknown"),
    stdin_id(THREAD_UNKNOWN),
    stdout_id(THREAD_UNKNOWN)


{
}

ProcessInfo::ProcessInfo(const ProcessInfo& pi)
    : tid(pi.tid), parent(pi.parent), name(pi.name), path(pi.path), stdin_id(pi.stdin_id), stdout_id(pi.stdout_id)
{
}

// Inherits from Parent
ProcessInfo::ProcessInfo(uint32_t tid, const ProcessInfo& parent) :
    tid(tid),
    parent(parent.tid),
    name(parent.name),
    path(parent.path),
    stdin_id(parent.stdin_id),
    stdout_id(parent.stdout_id)
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
