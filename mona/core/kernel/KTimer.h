#ifndef _MONA_KTIMER_
#define _MONA_KTIMER_

class KTimer : public KObject {

  public:
    KTimer(Thread* thread, uint32_t interval)
    {
        this->thread    = thread;
        this->interval  = interval;
        this->waitTick  = 0;
    }

    virtual ~KTimer()
    {
    }

  public:

    intptr_t getType() const
    {
        return KTIMER;
    }

    intptr_t checkSecurity(Thread* thread)
    {
        return 0;
    }

    bool timer(uint32_t now) {

        return now >= waitTick;
    }

    Thread* getThread() {

        return this->thread;
    }

    void setNextTimer(uint32_t systemTick)
    {
        this->waitTick = this->interval + systemTick;
    }

  private:
    Thread* thread;
    uint32_t interval;
    uint32_t waitTick;
};

#endif
