// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __GUISERVER_EFFECT_H__
#define __GUISERVER_EFFECT_H__

#include <gui/messages.h>

extern void CreationEffect(guiserver_window* w);
extern void DestructionEffect(guiserver_window* w);
extern void ExpansionEffect(int start_x, int start_y, int x, int y, int width, int height);

#endif  // __GUISERVER_EFFECT_H__
