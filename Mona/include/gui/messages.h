// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __GUI_MESSAGES_H__
#define __GUI_MESSAGES_H__

enum
{
	MSG_GUISERVER_GETFONT = 0x4000,
	MSG_GUISERVER_DECODEIMAGE,
	MSG_GUISERVER_DISPOSEIMAGE,
	MSG_GUISERVER_SETWALLPAPER,
	MSG_GUISERVER_REFRESHWALLPAPER,
	MSG_GUISERVER_DECOMPRESSBZ2FILE
};

#define MAKE_DWORD(a, b) (((unsigned int)(unsigned short)(a)) + (((unsigned int)(unsigned short)(b)) << 16))
#define GET_X_DWORD(a) ((int)(short)(a & 0xffff))
#define GET_Y_DWORD(a) ((int)(short)((a) >> 16))

#endif  // __GUI_MESSAGES_H__
