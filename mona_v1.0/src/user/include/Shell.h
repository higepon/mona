#ifndef _MONA_SHELL_
#define _MONA_SHELL_
#include <types.h>

/*----------------------------------------------------------------------
    Shell
----------------------------------------------------------------------*/
class Shell {

  public:
    Shell();
    virtual ~Shell();

  public:
    int onKeyDown(int keycode, int modifiers);

  protected:
    void commandChar(char c);
    void commandExecute();
    void commandTerminate();
    void backspace();

  protected:
    char commandLine_[1024];
    int position_;
};

#endif
