#ifndef _MONA_USERLIB_INPUTSTREAM_
#define _MONA_USERLIB_INPUTSREEAM_

#include <sys/types.h>

namespace MonAPI {

/*----------------------------------------------------------------------
    InputStream
----------------------------------------------------------------------*/
interface InputStream {

  public:
    virtual int open()   = 0;
    virtual void close() = 0;
    virtual int read(byte* buf, int size) = 0;
    virtual dword getReadSize() const = 0;
};

}

#endif
