// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __GUISERVER_FILE_H__
#define __GUISERVER_FILE_H__

#include "MemoryInfo.h"

extern MemoryInfo* ReadFile(const char* file, bool prompt = false);

#endif  // __GUISERVER_FILE_H__
