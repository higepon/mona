#ifndef _MONA_SHELL_
#define _MONA_SHELL_
#include <sys/types.h>
#include <monapi.h>
#include <monapi/CString.h>

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
    void putHistory(const MonAPI::CString& command);
    MonAPI::CString getHistory();
    int isInternalCommand(const MonAPI::CString& command);
    void internalCommandExecute(int command, CommandOption* option);
    int executeProcess(const MonAPI::CString& path, const MonAPI::CString& name ,CommandOption* option);

  protected:
    char commandLine_[1024];
    int position_;
    HList<MonAPI::CString> history;
};

#endif
