/*----------------------------------------------------------------------
    FileInputStream
----------------------------------------------------------------------*/

#include <monapi.h>

namespace MonAPI {

FileInputStream::FileInputStream(char* file) : file_(file), fileSize_(0), isOpen_(false) {
}

FileInputStream::~FileInputStream() {
}

int FileInputStream::open() {

    char file[256];
    int result;

    if (isOpen_) {
        return 0;
    }

    strncpy(file, file_, 256);
    result = syscall_file_open(file, FILE_OPEN_READ, &fileSize_);

    if (result == 0) {
        isOpen_ = true;
    }

    return result;
}

dword FileInputStream::getFileSize() const {
    return fileSize_;
}

dword FileInputStream::getReadSize() const {
    return readSize_;
}

int FileInputStream::read(byte* buf, int size) {
    return syscall_file_read((char*)buf, size, &readSize_);
}

void FileInputStream::close() {

    if (!isOpen_) {
        return;
    }

    syscall_file_close();
    isOpen_ = false;
    return;
}

}
