#ifndef _MONA_USERLIB_FILE_OUTPUTSTREAM_
#define _MONA_USERLIB_FILE_OUTPUTSTREAM_

#include <types.h>
#include <OutputStream.h>

/*----------------------------------------------------------------------
    FileOutputStream
----------------------------------------------------------------------*/
class FileOutputStream : public OutputStream {

  public:
    FileOutputStream(char* file);
    FileOutputStream(char* file, bool append);
    virtual ~FileOutputStream();

  public:
    virtual int open();
    virtual void close();
    virtual int write(byte* buf, int size);
    virtual dword getWriteSize() const;

  protected:
    char* file_;
    bool isOpen_;
    dword fileSize_;
    dword writeSize_;
    bool append_;
};
#endif
