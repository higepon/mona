// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __SYSTEM_MONA_INFO_H__
#define __SYSTEM_MONA_INFO_H__

#ifdef MONA
#include <monapi.h>
#endif

namespace System { namespace Mona
{
    uint32_t* gui_enum_windows(int* num);
}}

#endif  // __SYSTEM_MONA_INFO_H__
