// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __GUISERVER_IMAGE_H__
#define __GUISERVER_IMAGE_H__

#include <gui/messages.h>
#include <sys/types.h>
#include <monapi/cmemoryinfo.h>
#include <monapi/CString.h>

extern guiserver_bitmap* CreateBitmap(int width, int height, unsigned int background);
extern guiserver_bitmap* ReadBitmap(monapi_cmemoryinfo* mi);
extern guiserver_bitmap* ReadJPEG(monapi_cmemoryinfo* mi);
extern guiserver_bitmap* ReadImage(const MonAPI::CString& file, bool prompt = false);
extern void DrawImage(guiserver_bitmap* dst, guiserver_bitmap* img, int spx, int spy, int ix, int iy, int iw, int ih, int transparent);
extern void FillColor(guiserver_bitmap* bmp, unsigned int color);
extern bool ImageHandler(MessageInfo* msg);

#endif  // __GUISERVER_IMAGE_H__
