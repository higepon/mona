#ifndef __FILE_SERVER_STK5S_H__
#define __FILE_SERVER_STK5S_H__

#include <monapi/cmemoryinfo.h>

typedef long long int64_t;

extern int64_t GetST5DecompressedSize(monapi_cmemoryinfo* mi);
extern monapi_cmemoryinfo* ST5Decompress(monapi_cmemoryinfo* mi);
extern monapi_cmemoryinfo* ST5DecompressFile(const char* file, bool prompt = false);

#endif  // __FILE_SERVER_STK5S_H__
