#ifndef _MONA_USERLIB_SERVER_
#define _MONA_USERLIB_SERVER_

#include <types.h>

/*----------------------------------------------------------------------
    Server
----------------------------------------------------------------------*/
class Server {

  public:
    virtual void service() = 0;

};
#endif
