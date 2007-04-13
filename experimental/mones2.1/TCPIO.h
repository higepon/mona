//$Id$
#pragma once
#include <monapi.h>
namespace mones{

// See Tino's lecture of callback functions.
template <class T, void(T::*func)()>
void __fastcall this2fast(void* p) {
  (((T*)p)->*func)();
}

class TCPStream{
protected:
    uint32_t id;
public:    
    TCPStream(){  
        id=syscall_mthread_create_with_arg(this2fast<TCPStream,&TCPStream::ThreadMain>, this);
    }
    void** buffer;
    virtual void ThreadMain(){};
};

class TCPIn : public TCPStream
{
public:
    void ThreadMain();
};

class TCPOut : public TCPStream
{
public:    
    void ThreadMain();
};

};
