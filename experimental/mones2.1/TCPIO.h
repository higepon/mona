#pragma once

namespace mones{

template <class T, void(T::*func)()>
void __fastcall this2fast(void* p) {
  (((T*)p)->*func)();
}

class TCPStream{
    void** buffer;
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
