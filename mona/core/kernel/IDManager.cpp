#include "IDManager.h"
#include "global.h"
#include <sys/error.h>

IDManager::IDManager() : id(1000)
{
}

IDManager::~IDManager()
{
}

KObject* IDManager::get(int objectID, Thread* who)
{
    if (!tree.contains(objectID))
    {
        this->lastError = IDM_OBJECT_NOT_FOUND;
        return NULL;
    }
    KObject* object = tree.get(objectID);
    if (object->checkSecurity(who) != 0)
    {
        this->lastError = IDM_SECURITY_ERROR;
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
    object->setReferance();
    object->setId(id);
    return id;
}

void IDManager::returnID(int id)
{
    KObject* object = tree.remove(id);
    object->cancelReferance();
    return;
}
