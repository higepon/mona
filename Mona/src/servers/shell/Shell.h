#ifndef _MONA_SHELL_
#define _MONA_SHELL_

#include <sys/types.h>
#include <monapi.h>
#include <monapi/CString.h>
#include <gui/System/Array.h>

/*----------------------------------------------------------------------
    Shell
----------------------------------------------------------------------*/
class Shell {

  public:
    Shell();
    virtual ~Shell();

  public:
    void onKeyDown(int keycode, int modifiers);

  protected:
    void commandChar(char c);
    void commandExecute();
    void commandTerminate();
    void backspace();
    void putHistory(const MonAPI::CString& command);
    MonAPI::CString getHistory();
    int isInternalCommand(const MonAPI::CString& command);
    void internalCommandExecute(int command, _A<MonAPI::CString> args);
    _A<MonAPI::CString> parseCommandLine();
    int applicationList();

  protected:
    char commandLine_[1024];
    int position_;
    HList<MonAPI::CString> history;
    HList<MonAPI::CString> apps;
};

#endif
