#ifndef _MONA_KOBJECT_
#define _MONA_KOBJECT_

#include <sys/types.h>

class Thread;

class KObject
{
public:
    KObject();
    virtual ~KObject();

public:
    virtual intptr_t getReferanceCount() const;
    virtual intptr_t getType() const = 0;
    virtual void addRef();
    virtual bool releaseRef();

    virtual intptr_t getId() const {return id_;}
    virtual void setId(uint32_t id) { id_ = id;}
    void setOwner(Thread* owner);
    Thread* getOwner() const;

private:
    int referanceCount_;
    intptr_t id_;
    Thread* owner_;
public:
    enum
    {
        KOBJECT,
        KMUTEX,
        USER_SEMAPHORE,
        KTIMER,
        CONDITION
    };
};

#endif
