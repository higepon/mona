#ifndef _MONA_SHELL_SERVER_
#define _MONA_SHELL_SERVER_
#include <sys/types.h>
#include <monapi.h>

using namespace MonAPI;
/*----------------------------------------------------------------------
    ShellServer
----------------------------------------------------------------------*/
class ShellServer : public Server {

  public:
    ShellServer();
    virtual ~ShellServer();

  public:
    void service();
};
#endif
