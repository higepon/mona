// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __GUISERVER_BZIP2_H__
#define __GUISERVER_BZIP2_H__

#include "MemoryInfo.h"

typedef long long int64_t;

extern int64_t GetBZ2DecompressedSize(MemoryInfo* mi);
extern MemoryInfo* BZ2Decompress(MemoryInfo* mi);
extern MemoryInfo* BZ2DecompressFile(const char* file, bool prompt = false);

#endif
