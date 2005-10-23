// This file was created by `gcjh -stubs'. -*- c++ -*-
//
// This file is intended to give you a head start on implementing native
// methods using CNI.
// Be aware: running `gcjh -stubs ' once more for this class may
// overwrite any edits you have made to this file.

#include <java/awt/Image.h>
#include <gcj/cni.h>
#ifdef MONA
#include <monapi.h>
#include <monapi/messages.h>
#include <servers/gui.h>
#else
#include <stdlib.h>
#endif

static char* get_string(jstring s) {
	int len = s->length();
	char* cstr = new char[len + 1];
	jchar* ss = JvGetStringChars(s);
	for (int i = 0; i < len; i++)
		cstr[i] = (char)ss[i];
	cstr[len] = '\0';
	return cstr;
}


void
java::awt::Image::create (::java::awt::Image *image, ::java::lang::String *path)
{
#ifdef MONA
	// GUIサーバー上でビットマップをデコードする
	char* cpath = get_string(path);
	MessageInfo msg;
	if (MonAPI::Message::sendReceive(&msg, monapi_get_server_thread_id(ID_GUI_SERVER), 
		MSG_GUISERVER_DECODEIMAGE, 0, 0, 0, get_string(path)))
	{
		printf("%s:%d:ERROR: can not connect to GUI server!\n", __FILE__, __LINE__);
		delete(cpath);
		return;
	}
	if (msg.arg2 == 0) {
		delete(cpath);
		return;
	}
	
	// GUIサーバー上のビットマップオブジェクトを生成する
	guiserver_bitmap* bitmap = (guiserver_bitmap*)MonAPI::MemoryMap::map(msg.arg2);
	if (bitmap == NULL) {
		printf("%s:%d:ERROR: can not get image data!\n", __FILE__, __LINE__);
		delete(cpath);
		return;
	}
	
	image->width = bitmap->Width;
	image->width = bitmap->Height;
	image->malloc(bitmap->Width, bitmap->Height);
	return;
#else
	char* cpath = get_string(path);
	image->width = 0;
	image->width = 0;
	return;
#endif
}


