#ifndef _MONA_KTIMER_
#define _MONA_KTIMER_

class KTimer : public KObject {

  public:
    KTimer(Thread* thread, uint32_t interval, bool isOneShot = false) :
        thread_(thread),
        interval_(interval),
        waitTick_(0),
        isOneShot_(isOneShot)
    {
    }

    virtual ~KTimer()
    {
    }

  public:

    bool isOneShot() const {
        return isOneShot_;
    }

    intptr_t getType() const
    {
        return KTIMER;
    }

    bool timer(uint32_t now)
    {

        return now >= waitTick_;
    }

    void setNextTimer(uint32_t systemTick)
    {
        waitTick_ = interval_ + systemTick;
    }

    Thread* getOwnerThread() const
    {
        return thread_;
    }

  private:
    Thread* thread_;
    uint32_t interval_;
    uint32_t waitTick_;
    const bool isOneShot_;
};

#endif
