// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __GUISERVER_IMAGE_H__
#define __GUISERVER_IMAGE_H__

#include <monapi/cmemoryinfo.h>

class ImageInfo : public monapi_cmemoryinfo
{
public:
    int Width, Height;

    ImageInfo();
};

extern ImageInfo* ReadBitmap(monapi_cmemoryinfo* mi);
extern ImageInfo* ReadJPEG(monapi_cmemoryinfo* mi);
extern ImageInfo* ReadImage(const char* file, bool prompt = false);

#endif  // __GUISERVER_IMAGE_H__
