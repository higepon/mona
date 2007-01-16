#ifndef _MONAPI_MONAAPPLICATION_
#define _MONAPI_MONAAPPLICATION_

#include <sys/types.h>

namespace MonAPI {

/*----------------------------------------------------------------------
    Receiver
----------------------------------------------------------------------*/
interface Receiver {
  public:
    Receiver() {}
    virtual ~Receiver() {}
    virtual void onKeyDown(int keycode, int modifiers) = 0;
    virtual void onMouseClick(int x, int y) = 0;
};

/*----------------------------------------------------------------------
    MonaApplication
----------------------------------------------------------------------*/
class MonaApplication : public Receiver {

  public:
    MonaApplication();
    virtual ~MonaApplication();

  public:
    virtual int main(List<char*>* pekoe) = 0;

    /* default implementation */
    virtual void onKeyDown(int keycode, int modifiers) {
    }

    virtual void onMouseClick(int x, int y) {
    }

    virtual inline uint32_t getMyPID() {
        return mypid_;
    }

    virtual void run();

  protected:
    uint32_t mypid_;
};

extern MonaApplication* monaApp;

}

#endif
