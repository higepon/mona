// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __GUISERVER_WINDOW_H__
#define __GUISERVER_WINDOW_H__

#include <servers/gui.h>
#include <sys/types.h>

extern guiserver_window* CreateWindow();
extern guiserver_window* GetWindowPointer(dword handle);
extern bool DisposeWindow(dword handle);
extern void DisposeWindowFromThreadID(dword tid);
extern void DrawWindow(guiserver_window* w, bool draw_screen = true);
extern void MoveWindow(guiserver_window* w, int x, int y);
extern guiserver_window* GetTargetWindow(int x, int y);
extern bool WindowHandler(MessageInfo* msg);

#endif  // __GUISERVER_WINDOW_H__
