#include "ProcessInfo.h"


ProcessInfo::ProcessInfo(int dummy /*= NULL */) : tid(THREAD_UNKNOWN), parent(THREAD_UNKNOWN)
{
}

ProcessInfo::ProcessInfo(const ProcessInfo& pi)
    : tid(pi.tid), parent(pi.parent), name(pi.name), path(pi.path)
{
}

ProcessInfo::ProcessInfo(dword tid, dword parent, const MonAPI::CString& name, const MonAPI::CString& path)
    : tid(tid), parent(parent), name(name), path(path)
{
}

ProcessInfo& ProcessInfo::operator =(const ProcessInfo& pi)
{
    this->tid    = pi.tid;
    this->parent = pi.parent;
    this->name   = pi.name;
    this->path   = pi.path;
    return *this;
}
