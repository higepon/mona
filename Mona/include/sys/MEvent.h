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
        NONE             = 64,
        TIMER            = 20,
        TIMER_MESSAGE    = 19,
        MESSAGE          = 22,
        MUTEX_UNLOCKED   = 27,
        INTERRUPT_HIGH   = 0,
        INTERRUPT_MIDDLE = 7,
        INTERRUPT_LOW    = 17
    };
};

#endif
