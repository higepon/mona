#ifndef _MONA_SHELL_
#define _MONA_SHELL_

#include <sys/types.h>
#include <monapi.h>
#include <monapi/CString.h>
#include <gui/System/Array.h>

#define SVR "Shell Server"
#define FONT_WIDTH   8
#define FONT_HEIGHT 16
#define FOREGROUND 0x000000
#define BACKGROUND 0xffffff
#define APPSDIR  "/APPS"
#define STARTDIR "/APPS"

/*----------------------------------------------------------------------
    Shell
----------------------------------------------------------------------*/
class Shell {

  public:
    Shell(bool callAutoExec);
    virtual ~Shell();

  public:
    void run();
    void onKeyDown(int keycode, int modifiers);
    void printPrompt(const MonAPI::CString& prefix = NULL);
    void drawCaret(bool erase = false);
    inline bool getHasExited() { return this->hasExited; }

  protected:
    void backspace();
    void commandChar(char c);
    void commandExecute(bool prompt);
    void commandTerminate();
    void putHistory(const MonAPI::CString& command);
    MonAPI::CString getHistory();
    int isInternalCommand(const MonAPI::CString& command);
    bool internalCommandExecute(int command, _A<MonAPI::CString> args);
    _A<MonAPI::CString> parseCommandLine();
    int makeApplicationList();
    MonAPI::CString getParentDirectory(const MonAPI::CString& dir);
    MonAPI::CString mergeDirectory(const MonAPI::CString& dir1, const MonAPI::CString& dir2);
    void printFiles(const MonAPI::CString& dir);
    void executeMSH(const MonAPI::CString& msh);
    void checkCaretPosition();

  protected:
    char commandLine[1024];
    int position;
    HList<MonAPI::CString> history;
    HList<MonAPI::CString> apps;
    MonAPI::CString current;
    bool hasExited, callAutoExec;
    dword waiting;
    MonAPI::Screen screen;
    int prevX, prevY;
};

#endif
