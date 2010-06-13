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
//     logprintf("%s %s:%d obj=%x\n", __func__, __FILE__, __LINE__, obj);
//     logprintf("%s %s:%d thread=%x\n", __func__, __FILE__, __LINE__, obj->getThread());
//     logprintf("%s %s:%d tinfo=%x\n", __func__, __FILE__, __LINE__, obj->getThread()->tinfo);
//     logprintf("%s %s:%d process=%x\n", __func__, __FILE__, __LINE__, obj->getThread() == NULL ? 0x11111111 : (int)obj->getThread()->tinfo->process);
//     logprintf("%s %s:%d thread2=%x\n", __func__, __FILE__, __LINE__, obj->getThread() == NULL ? 0x11111111 : (int)obj->getThread()->tinfo->thread);
// To check resource leak.
#if 1
    logprintf("obj=%x type=%d name=%s\n", obj, obj->getType(), obj->getOwner() == NULL ? "KERNEL" : obj->getOwner()->getName());
#endif
    if (obj->getType() == typeToFind) {
        foundCount++;
    }
}

void IDManager::foreachKObject(void (*func)(int id, KObject* object))
{
    logprintf("foreach start %s \n", g_currentThread->thread->tinfo->process->getName());
    tree.traverse(func);
    logprintf("foreach end %s \n", g_currentThread->thread->tinfo->process->getName());
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

intptr_t IDManager::allocateID(Process* owner, KObject* object)
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    int id = this->id++;
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    ASSERT(!tree.contains(id));
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    tree.add(id, object);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    object->addRef();
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    object->setId(id);
    object->setOwner(owner);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    return id;
}

bool IDManager::returnID(int id)
{
    KObject* object = tree.remove(id);
    return object->releaseRef();
}
