#ifndef _MONA_KOBJECT_
#define _MONA_KOBJECT_

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

private:
    int referanceCount;

public:
    static const int KOBJECT;
    static const int THREAD;
};

#endif
