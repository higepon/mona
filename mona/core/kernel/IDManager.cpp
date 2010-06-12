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
#if 1
    logprintf("obj=%x type=%d name=%s\n", obj, obj->getType(), obj->getThread() == NULL ? "KERNEL" : obj->getThread()->tinfo->process->getName());
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

intptr_t IDManager::allocateID(Thread* owner, KObject* object)
{
    int id = this->id++;
    ASSERT(!tree.contains(id));
    tree.add(id, object);
    object->addRef();
    object->setId(id);
    object->setThread(owner);
    return id;
}

bool IDManager::returnID(int id)
{
    KObject* object = tree.remove(id);
    return object->releaseRef();
}
