#ifndef _MONAPI_SERVER_
#define _MONAPI_SERVER_

#include <sys/types.h>

namespace MonAPI {

/*----------------------------------------------------------------------
    Server
----------------------------------------------------------------------*/
class Server {

  public:
    Server();
    virtual ~Server();
    virtual void service() = 0;

};

}

#endif
