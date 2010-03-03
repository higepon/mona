#ifndef _MONA_MEVENT_
#define _MONA_MEVENT_

/*----------------------------------------------------------------------
    MEvent
----------------------------------------------------------------------*/
class MEvent
{
public:
    enum
    {
        NONE             = M_EVENT_NONE,
        SLEEP            = M_EVENT_SLEEP,
        TIMER_MESSAGE    = M_EVENT_TIMER_MESSAGE,
        MESSAGE          = M_EVENT_MESSAGE,
        MUTEX_UNLOCKED   = M_EVENT_MUTEX_UNLOCKED,
        CONDITION_NOTIFY = M_EVENT_CONDITION_NOTIFY,
        SEMAPHORE_UPPED  = M_EVENT_SEMAPHORE_UPPED,
        INTERRUPT_HIGH   = M_EVENT_INTERRUPT_HIGH,
        INTERRUPT_MIDDLE = M_EVENT_INTERRUPT_MIDDLE,
        INTERRUPT_LOW    = M_EVENT_INTERRUPT_LOW
    };
};

#endif
