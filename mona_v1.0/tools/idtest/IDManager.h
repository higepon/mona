#ifndef _MONA_ID_MANAGER_
#define _MONA_ID_MANAGER_

#include "KObject.h"
#include "BinaryTree.h"

class IDManager
{
public:
    IDManager();
    virtual ~IDManager();

public:
    KObject* get(int objectID, Thread* who);
    void returnID(int id);
    int allocateID(KObject* object);
    int getLastError() const;

private:
    BinaryTree<KObject*> tree;
    int lastError;
    int id;
};

#endif
