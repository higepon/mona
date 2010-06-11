#ifndef _MONA_KTIMER_
#define _MONA_KTIMER_

class KTimer : public KObject {

  public:
    KTimer(uint32_t interval)
    {
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

    bool timer(uint32_t now)
    {

        return now >= waitTick;
    }

    void setNextTimer(uint32_t systemTick)
    {
        this->waitTick = this->interval + systemTick;
    }

  private:
    uint32_t interval;
    uint32_t waitTick;
};

#endif
