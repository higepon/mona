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

    int getType() const
    {
        return KTIMER;
    }

    int checkSecurity(Thread* thread)
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
