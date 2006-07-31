#ifndef _MONA_USERLIB_SERVER_
#define _MONA_USERLIB_SERVER_

#include <sys/types.h>

namespace MonAPI {

/*----------------------------------------------------------------------
    Server
----------------------------------------------------------------------*/
class Server {

  public:
    virtual void service() = 0;

};

}

#endif
