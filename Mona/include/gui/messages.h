// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __GUI_MESSAGES_H__
#define __GUI_MESSAGES_H__

enum
{
	MSG_GUISERVER_GETFONT = 0x4000,
	MSG_GUISERVER_DECODEIMAGE,
	MSG_GUISERVER_OBSOLETE1,  //MSG_GUISERVER_DISPOSEHANDLE
	MSG_GUISERVER_SETWALLPAPER,
	MSG_GUISERVER_DRAWWALLPAPER,
	MSG_GUISERVER_OBSOLETE2,  // MSG_GUISERVER_DECOMPRESSBZ2
	MSG_GUISERVER_OBSOLETE3,  // MSG_GUISERVER_DECOMPRESSBZ2FILE
	MSG_GUISERVER_CREATEBITMAP,
	
	MSG_GUISERVER_CREATEWINDOW = 0x4100,
	MSG_GUISERVER_DRAWWINDOW,
	
	MSG_GUISERVER_MOUSEMOVE = 0x4200,
	MSG_GUISERVER_MOUSEDOWN,
	MSG_GUISERVER_MOUSEUP,
	
	MSG_GUISERVER_KEYDOWN = 0x4300,
	MSG_GUISERVER_KEYUP,
	MSG_GUISERVER_KEYPRESS
};

typedef struct
{
	unsigned int Handle;
	int Width, Height;
	unsigned int Data[];
} guiserver_bitmap;

typedef struct
{
	unsigned int Handle, Parent, Owner;
	int X, Y, Width, Height, OffsetX, OffsetY, Opacity;
	unsigned int Flags, TransparencyKey;
	void* __reserved1, * __reserved2, * __reserved3;
	guiserver_bitmap Buffer;
} guiserver_window;

#define MAKE_DWORD(a, b) (((unsigned int)(unsigned short)(a)) + (((unsigned int)(unsigned short)(b)) << 16))
#define GET_X_DWORD(a) ((int)(short)(a & 0xffff))
#define GET_Y_DWORD(a) ((int)(short)((a) >> 16))

#endif  // __GUI_MESSAGES_H__
