
#include <stdlib.h>
#include <stdio.h>

extern "C" int sleep(int time);
extern "C" int fork();
void child();
void parent();

typedef unsigned char byte;
typedef unsigned int  dword;

int main(int argc, char** argv) {

    int pid;

    /* fork fail */
    if ((pid = fork()) < 0) {

        printf("can not fork())\n");
        exit(-1);
    }

    if (pid == 0) {

        child();
    } else {

        parent();
    }

    return 0;
}

void parent() {

    printf("parent");
    fflush(stdout);
    sleep(5);
    printf("parent");
}

void child() {

    printf("child");
    printf("child");
}

class InputStream {

  public:
    virtual int  read(byte* buf, int size) = 0;
    virtual byte read()  = 0;
    virtual void close() = 0;
    virtual bool isEndOfStream() = 0;
};

class DRMNInputStream : public InputStream {

  public:
    DRMNInputStream(byte* start, int size);

  public:
    virtual int  read(byte* buf, int size);
    virtual byte read();
    virtual void close();
    virtual bool isEndOfStream();

  private:
    byte* start_;
    dword offset_;
    dword end_;


};

DRMNInputStream::DRMNInputStream(byte* start, int size) {

    start_  = start;
    offset_ = 0;
    end_    = (dword)start + size;
}

byte DRMNInputStream::read() {

    byte result = start_[offset_];
    if (offset_ < end_) offset_++;
    return result;
}

int DRMNInputStream::read(byte* buf, int size) {

    int bufferSize = offset_ - (dword)start + 1;
    int readSize;

    /* check size */
    if (size <= 0 || bufferSize <= 0) return 0;

    /* read size */
    if (size > bufferSize) readSize = bufferSize;
    else readSize = size;

    /* read */
    memcpy(buf, (byte*)((dword)start_ + offset_), readSize);
    offset_ += readSize;

    return readSize;
}

bool DRMNInputStream::isEndOfStream() {

    return (offset_ == end_);
}


void DRMNInputStream::close() {


}
