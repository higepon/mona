#ifndef _MONA_KTIMER_
#define _MONA_KTIMER_

class KTimer : public KObject {

  public:
    KTimer(Thread* thread, uint32_t interval) : thread_(thread), interval_(interval), waitTick_(0)
    {
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
};

#endif
