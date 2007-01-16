// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __GUISERVER_IMAGE_H__
#define __GUISERVER_IMAGE_H__

#include <servers/gui.h>
#include <sys/types.h>
#include <monapi/cmemoryinfo.h>
#include <monapi/CString.h>

extern guiserver_bitmap* CreateBitmap(int width, int height, unsigned int background);
extern guiserver_bitmap* GetBitmapPointer(uint32_t handle);
extern bool DisposeBitmap(uint32_t handle);
extern guiserver_bitmap* ReadBitmap(monapi_cmemoryinfo* mi);
extern guiserver_bitmap* ReadJPEG(monapi_cmemoryinfo* mi);
extern guiserver_bitmap* ReadImage(const MonAPI::CString& file, bool prompt = false);
extern guiserver_bitmap* ResizeBitmap(guiserver_bitmap* bmp, int width, int height);
extern void FillColor(guiserver_bitmap* bmp, unsigned int color);
extern void DrawImage(guiserver_bitmap* dst, guiserver_bitmap* src, int x = 0, int y = 0, int sx = 0, int sy = 0, int sw = -1, int sh = -1, unsigned int transparencyKey = 0, int opacity = 255);
extern bool ImageHandler(MessageInfo* msg);

#endif  // __GUISERVER_IMAGE_H__
