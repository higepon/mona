
#ifndef _MONA_SHELL_
#define _MONA_SHELL_

#include <sys/types.h>
#include <monapi.h>
#include <monapi/CString.h>
#include <monapi/Array.h>

#define SVR "Shell Server"
#define FONT_WIDTH   8
#define APPSDIR  "/APPS"

/*----------------------------------------------------------------------
    Shell
----------------------------------------------------------------------*/
class Shell {

  public:
    Shell(uint32_t outHandle);
    virtual ~Shell();

  public:
    void run();
    void onKeyDown(int keycode, int modifiers);

  protected:
    void backspace();
    void commandChar(char c);
    bool isPipeCommand();
    void commandExecutePipe();
    void commandExecute(bool prompt);
    bool commandExecute(_A<MonAPI::CString> args, uint32_t stdout_id, uint32_t stdin_id);
    void commandTerminate();
    int isInternalCommand(const MonAPI::CString& command);
    bool internalCommandExecute(int command, _A<MonAPI::CString> args);
    _A<MonAPI::CString> parseCommandLine();
    int makeApplicationList();
    MonAPI::CString getParentDirectory(const MonAPI::CString& dir);
    MonAPI::CString mergeDirectory(const MonAPI::CString& dir1, const MonAPI::CString& dir2);
    void printFiles(const MonAPI::CString& dir);
    void executeMSH(const MonAPI::CString& msh);
    bool changeDirecotory(const MonAPI::CString& path);

    int writeToOutBuffer();
    int prompt(bool newline = true);
    int formatWrite(const char* format, ...);
    enum
    {
        CURSOR_UP    = 'A',
        CURSOR_DOWN  = 'B',
        CURSOR_RIGHT = 'C',
        CURSOR_LEFT  = 'D',
        OUT_BUFFER_SIZE = 512,
    };

    char outbuffer_[OUT_BUFFER_SIZE];

  protected:
    char commandLine_[1024];
    int position_;
    HList<MonAPI::CString> apps_;
    bool doExec_;
    uint32_t waiting_;
    MonAPI::Screen screen_;
    HList<MonAPI::CString> history_;
    MonAPI::CString startDirectory_;
    MonAPI::CString currentDirectory_;
    MonAPI::Stream* inStream_;
    MonAPI::Stream* outStream_;
    MonAPI::TerminalUtil* terminal_;
};

#endif
