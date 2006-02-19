#ifndef __LIBNET_H__
#define __LIBNET_H__

#include <sys/types.h>
#include <vector>
typedef std::vector<byte> Buffer;
void tcpInitialize();
dword tcpConnect(byte a, byte b, byte c, byte d, dword port);
dword tcpSendMemoryMap(dword handle, const byte* data, dword length);
dword tcpSend(dword handle, const byte* data, dword length);
Buffer tcpRead(dword handle);

#endif // __LIBNET_H__
