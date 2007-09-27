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
    virtual int checkSecurity(Thread* thread);
    virtual int getReferanceCount() const;
    virtual int getType() const;
    virtual void setReferance();
    virtual void cancelReferance();

    uint32_t getId() const {return id;}
    void setId(uint32_t id) {this->id = id;}

private:
    int referanceCount;

public:
    uint32_t id;
    enum
    {
        KOBJECT,
        THREAD,
        KMUTEX,
        USER_SEMAPHORE,
        KTIMER
    };
};

#endif
