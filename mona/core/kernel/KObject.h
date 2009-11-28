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
    virtual intptr_t checkSecurity(Thread* thread) = 0;
    virtual intptr_t getReferanceCount() const;
    virtual intptr_t getType() const = 0;
    virtual void setReferance();
    virtual void cancelReferance();

    virtual intptr_t getId() const {return id;}
    virtual void setId(uint32_t id) {this->id = id;}

private:
    int referanceCount;

public:
    intptr_t id;
    enum
    {
        KOBJECT,
        THREAD,
        KMUTEX,
        USER_SEMAPHORE,
        KTIMER,
        CONDITION
    };
};

#endif
