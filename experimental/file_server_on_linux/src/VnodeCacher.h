#ifndef __VNODECACHER_H__
#define __VNODECACHER_H__

#include "vnode.h"
#include <stdio.h>
#include <map>
#include <vector>
#include <string>

typedef std::map<std::string, Vnode*> EntriesMap;
typedef std::map<Vnode*, EntriesMap*> DirectoriesMap;

class VnodeCacher
{
public:
    VnodeCacher();
    virtual ~VnodeCacher();

public:
    Vnode* lookup(Vnode* directory, const std::string& name);
    void add(Vnode* directory, const std::string& name, Vnode* entry);
    void remove(Vnode* directory, const std::string& name);

protected:
    DirectoriesMap* directories_;
};

#endif // __VNODECACHER_H__
