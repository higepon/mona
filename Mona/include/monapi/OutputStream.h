#ifndef _MONA_USERLIB_OUTPUTSTREAM_
#define _MONA_USERLIB_OUTPUTSTREAM__

#include <sys/types.h>

namespace MonAPI {

/*----------------------------------------------------------------------
    OutputStream
----------------------------------------------------------------------*/
interface OutputStream {

  public:
    virtual int open()   = 0;
    virtual void close() = 0;
    virtual int write(byte* buf, int size) = 0;
};

}

#endif
