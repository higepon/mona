#ifndef _MONA_THREAD_PRIORITY_
#define _MONA_THREAD_PRIORITY__

/*----------------------------------------------------------------------
    ThreadPriority
----------------------------------------------------------------------*/
class ThreadPriority
{
public:
    ThreadPriority() {}
    virtual ~ThreadPriority() {}
public:
    enum
    {
        Max    = 0,
        Normal = 30,
        Min    = 63
    };
};

#endif
