#ifndef _MONA_USERLIB_MONAAPPLICATION_
#define _MONA_USERLIB_MONAAPPLICATION_

#include <sys/types.h>

namespace MonAPI {

/*----------------------------------------------------------------------
    Receiver
----------------------------------------------------------------------*/
interface Receiver {
  public:
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

    virtual inline dword getMyPID() {
        return mypid_;
    }

    virtual void run();

  protected:
    dword mypid_;
};

extern MonaApplication* monaApp;

}

#endif
