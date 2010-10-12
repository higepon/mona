#include "IDManager.h"
#include "global.h"
#include <sys/error.h>

IDManager::IDManager() : id(1000)
{
}

IDManager::~IDManager()
{
}

static int foundCount = 0;
static int typeToFind = 0;

static void accumCount(int id, KObject* obj)
{
// To check resource leak.
#if 0
    logprintf("obj=%x\n", obj);
    logprintf("obj=%x type=%d name=%s\n", obj, obj->getType(), obj->getOwner() == NULL ? "KERNEL" : obj->getOwner()->getName());
#endif
    if (obj->getType() == typeToFind) {
        foundCount++;
    }
}

void IDManager::foreachKObject(void (*func)(int id, KObject* object))
{
    tree.traverse(func);
}

intptr_t IDManager::getCount(int type)
{
    foundCount = 0;
    typeToFind = type;
    tree.traverse(&accumCount);
    return foundCount;
}

KObject* IDManager::get(int objectID, int type)
{
    if (!tree.contains(objectID))
    {
        this->lastError = M_OBJECT_NOT_FOUND;
        return NULL;
    }
    KObject* object = tree.get(objectID);
    if (object->getType() != type) {
        this->lastError = M_BAD_ARG;
        return NULL;
    }
    return object;
}

intptr_t IDManager::getLastError() const
{
    return this->lastError;
}

intptr_t IDManager::allocateID(KObject* object)
{
    int id = this->id++;
    ASSERT(!tree.contains(id));
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
