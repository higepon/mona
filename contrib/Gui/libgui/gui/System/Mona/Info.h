// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __SYSTEM_MONA_INFO_H__
#define __SYSTEM_MONA_INFO_H__

#ifdef MONA
#include <monapi.h>
#include <servers/gui.h>
#endif

namespace System { namespace Mona
{
    uint32_t* gui_enum_windows(int* num);
    int gui_get_window_title(uint32_t handle, char* buffer);
    int gui_move_window(uint32_t handle, int x, int y);
}}

#endif  // __SYSTEM_MONA_INFO_H__
