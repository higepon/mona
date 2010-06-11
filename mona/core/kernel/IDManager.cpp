#include "IDManager.h"
#include "global.h"
#include <sys/error.h>

IDManager::IDManager() : id(1000)
{
}

IDManager::~IDManager()
{
}

static int mutexCount = 0;

static void accumMutexCount(int id, KObject* obj)
{
    if (obj->getType() == KObject::KMUTEX) {
        mutexCount++;
    }
}

intptr_t IDManager::getCount(int type)
{
    mutexCount = 0;
    tree.traverse(&accumMutexCount);
    return mutexCount;
}

KObject* IDManager::get(int objectID, Thread* who, int type)
{
    if (!tree.contains(objectID))
    {
        this->lastError = IDM_OBJECT_NOT_FOUND;
        return NULL;
    }
    KObject* object = tree.get(objectID);
    if (object->getType() != type) {
        this->lastError = IDM_INVALID_TYPE;
        return NULL;
    }
    return object;
}

int IDManager::getLastError() const
{
    return this->lastError;
}

intptr_t IDManager::allocateID(KObject* object)
{
    int id = this->id++;
    tree.add(id, object);
    object->addRef();
    object->setId(id);
    return id;
}

bool IDManager::returnID(int id)
{
    KObject* object = tree.remove(id);
    return object->releaseRef();
}
