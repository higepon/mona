#ifndef _MONA_SHELL_
#define _MONA_SHELL_
#include <sys/types.h>
#include <monapi.h>

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
    void putHistory(const char* command);
    int isInternalCommand(const char* command);
    void internalCommandExecute(int command, CommandOption* option);
    char* getHistory();
    int executeProcess(const char* path, const char* name ,CommandOption* option);

  protected:
    char commandLine_[1024];
    int position_;
    List<char*>* history_;

  protected:
    static char* const PROMPT;
};

#endif
