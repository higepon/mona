#ifndef __VNODECACHER_H__
#define __VNODECACHER_H__

#include "vnode.h"
#include <stdio.h>
#include <map>
#include <string>

typedef std::map<std::string, vnode*> EntriesMap;
typedef std::map<vnode*, EntriesMap*> DirectoriesMap;

class VnodeCacher
{
public:
    VnodeCacher();
    virtual ~VnodeCacher();

public:
    vnode* lookup(vnode* directory, const std::string& name);
    void add(vnode* directory, const std::string& name, vnode* entry);
    void remove(vnode* directory, const std::string& name);

protected:
    DirectoriesMap* directories_;
};

#endif // __VNODECACHER_H__
