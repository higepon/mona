// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __GUISERVER_SCREEN_H__
#define __GUISERVER_SCREEN_H__

#include <monapi.h>

extern MonAPI::Screen* GetDefaultScreen();
extern bool InitScreen();
extern void DisposeScreen();
extern void DrawScreen(int x = 0, int y = 0, int w = -1, int h = -1);

#endif  // __GUISERVER_SCREEN_H__
