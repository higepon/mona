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
    KObject* get(int objectID, int type);
    bool returnID(int id);
    intptr_t allocateID(Process* owner, KObject* object);
    int getLastError() const;
    intptr_t getCount(int type);
    void foreachKObject(void (*func)(int id, KObject* object));

private:
    BinaryTree<KObject*> tree;
    int lastError;
    intptr_t id;
};

#endif
