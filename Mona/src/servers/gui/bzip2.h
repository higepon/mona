// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __GUISERVER_BZIP2_H__
#define __GUISERVER_BZIP2_H__

#include <monapi/cmemoryinfo.h>

typedef long long int64_t;

extern int64_t GetBZ2DecompressedSize(monapi_cmemoryinfo* mi);
extern monapi_cmemoryinfo* BZ2Decompress(monapi_cmemoryinfo* mi);
extern monapi_cmemoryinfo* BZ2DecompressFile(const char* file, bool prompt = false);

#endif
