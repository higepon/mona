/*----------------------------------------------------------------------
    FileOutputStream
----------------------------------------------------------------------*/

#include <monapi.h>

namespace MonAPI {

FileOutputStream::FileOutputStream(char* file) : file_(file), isOpen_(false), fileSize_(0), writeSize_(0), append_(false)
{
}

FileOutputStream::FileOutputStream(char* file, bool append) : file_(file), isOpen_(false), fileSize_(0), writeSize_(0), append_(append)
{
}

FileOutputStream::~FileOutputStream()
{
}

int FileOutputStream::open()
{
    char file[256];
    int result;

    if (isOpen_)
    {
        return 0;
    }

    strncpy(file, file_, 256);
    result = syscall_file_open(file, this->append_ ? FILE_OPEN_APPEND_WRITE : FILE_OPEN_NORMAL_WRITE, &fileSize_);

    if (result == 0)
    {
        isOpen_ = true;
        return result;
    } else if (result != FS_FILE_NOT_FOUND)
    {
        return result;
    }

    /* file not found, so create file */
    strncpy(file, file_, 256);
    printf("file create!!!!");
    result = syscall_file_create(file);

    if (result != 0)
    {
        return result;
    }

    strncpy(file, file_, 256);
    printf("file reopen[%s]!!!!", file);
    result = syscall_file_open(file, this->append_ ? FILE_OPEN_APPEND_WRITE : FILE_OPEN_NORMAL_WRITE, &fileSize_);

    if (result == 0)
    {
        isOpen_ = true;
    }
    return result;
}

int FileOutputStream::write(byte* buf, int size)
{
    return syscall_file_write((char*)buf, size, &writeSize_);
}

dword FileOutputStream::getWriteSize() const
{
    return writeSize_;
}

void FileOutputStream::close()
{
    if (!isOpen_)
    {
        return;
    }

    syscall_file_close();
    isOpen_ = false;
    return;
}

}
