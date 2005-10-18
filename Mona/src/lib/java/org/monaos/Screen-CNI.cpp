// This file was created by `gcjh -stubs'. -*- c++ -*-
//
// This file is intended to give you a head start on implementing native
// methods using CNI.
// Be aware: running `gcjh -stubs ' once more for this class may
// overwrite any edits you have made to this file.

#include <org/monaos/Screen.h>
#include <gcj/cni.h>
#ifdef MONA
#include <monapi.h>
#endif

jint
org::monaos::Screen::getWidth ()
{
#ifdef MONA
	volatile ScreenInfo info;
	syscall_get_vram_info(&info);
	return info.x;
#else
	return 0;
#endif
}


jint
org::monaos::Screen::getHeight ()
{
#ifdef MONA
	volatile ScreenInfo info;
	syscall_get_vram_info(&info);
	return info.y;
#else
	return 0;
#endif
}


jint
org::monaos::Screen::getBpp ()
{
#ifdef MONA
	volatile ScreenInfo info;
	syscall_get_vram_info(&info);
	return info.bpp;
#else
	return 0;
#endif
}


