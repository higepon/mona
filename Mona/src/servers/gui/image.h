// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __GUISERVER_IMAGE_H__
#define __GUISERVER_IMAGE_H__

#include "MemoryInfo.h"

extern MemoryInfo* ReadBitmap(MemoryInfo* mi);
extern MemoryInfo* ReadJPEG(MemoryInfo* mi);
extern MemoryInfo* ReadImage(const char* file, bool prompt = false);

#endif  // __GUISERVER_IMAGE_H__
