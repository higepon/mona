#ifndef _MONA_USERLIB_FILE_OUTPUTSTREAM_
#define _MONA_USERLIB_FILE_OUTPUTSTREAM_

#include <sys/types.h>
#include <monapi/OutputStream.h>

namespace MonAPI {

/*----------------------------------------------------------------------
    FileOutputStream
----------------------------------------------------------------------*/
class FileOutputStream : public OutputStream {

  public:
    FileOutputStream(const char* file);
    FileOutputStream(const char* file, bool append);
    virtual ~FileOutputStream();

  public:
    virtual int open();
    virtual void close();
    virtual int write(byte* buf, int size);
    virtual dword getWriteSize() const;

  protected:
    const char* file_;
    bool isOpen_;
    dword fileSize_;
    dword writeSize_;
    bool append_;
};

}

#endif
