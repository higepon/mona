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
    void printPrompt(const MonAPI::CString& prefix = NULL);

  protected:
    void commandChar(char c);
    void commandExecute(bool prompt);
    void commandTerminate();
    void backspace();
    void putHistory(const MonAPI::CString& command);
    MonAPI::CString getHistory();
    int isInternalCommand(const MonAPI::CString& command);
    void internalCommandExecute(int command, _A<MonAPI::CString> args);
    _A<MonAPI::CString> parseCommandLine();
    int makeApplicationList();
    MonAPI::CString getParentDirectory(const MonAPI::CString& dir);
    MonAPI::CString mergeDirectory(const MonAPI::CString& dir1, const MonAPI::CString& dir2);
    void printFiles(const MonAPI::CString& dir);
    void executeMSH(const MonAPI::CString& msh);

  protected:
    char commandLine_[1024];
    int position_;
    HList<MonAPI::CString> history;
    HList<MonAPI::CString> apps;
    MonAPI::CString current;
};

#endif
