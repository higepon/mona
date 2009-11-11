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
    void returnID(int id);
    intptr_t allocateID(KObject* object);
    int getLastError() const;

private:
    BinaryTree<KObject*> tree;
    int lastError;
    intptr_t id;
};

#endif
