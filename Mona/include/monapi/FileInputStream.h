#ifndef _MONA_USERLIB_FILE_INPUTSTREAM_
#define _MONA_USERLIB_FILE_INPUTSREEAM_

#include <sys/types.h>
#include <monapi/InputStream.h>

namespace MonAPI {

/*----------------------------------------------------------------------
    FileInputStream
----------------------------------------------------------------------*/
class FileInputStream : public InputStream {

  public:
    FileInputStream(char* file);
    virtual ~FileInputStream();

  public:
    virtual int open();
    virtual void close();
    virtual int read(byte* buf, int size);
    virtual dword getReadSize() const;
    virtual dword getFileSize() const;

  protected:
    char* file_;
    dword readSize_;
    volatile dword fileSize_;
    bool isOpen_;
};

}

#endif
