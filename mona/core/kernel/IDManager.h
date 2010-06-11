#ifndef _MONA_ID_MANAGER_
#define _MONA_ID_MANAGER_

#include <sys/BinaryTree.h>
#include "KObject.h"

class IDManager
{
public:
    IDManager();
    virtual ~IDManager();

public:
    KObject* get(int objectID, Thread* who, int type);
    bool returnID(int id);
    intptr_t allocateID(Thread* owner, KObject* object);
    int getLastError() const;

    intptr_t getCount(int type);

private:
    BinaryTree<KObject*> tree;
    int lastError;
    intptr_t id;
};

#endif
