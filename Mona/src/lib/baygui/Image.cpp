/*
Copyright (c) 2004 bayside
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "baygui.h"

/**
 コンストラクタ
 @param width 幅
 @param height 高さ
 */
Image::Image(int width, int height)
{
	this->width = this->height = 0;
	this->bitmap = NULL;
	
	// GUIサーバーを探す
	dword guisvrID = monapi_get_server_thread_id(ID_GUI_SERVER);
	if (guisvrID == THREAD_UNKNOWN) {
		printf("%s:%d:ERROR: can not connect to GUI server!\n", __FILE__, __LINE__);
		return;
	}

	// GUIサーバー上にビットマップを生成する
	MessageInfo msg;
	if (MonAPI::Message::sendReceive(&msg, guisvrID, MSG_GUISERVER_CREATEBITMAP, width, height, 0xffffffff)) {
		printf("%s:%d:ERROR: can not connect to GUI server!\n", __FILE__, __LINE__);
		return;
	}
	if (msg.arg2 == 0) return;

	// GUIサーバー上のビットマップオブジェクトを生成する
	this->bitmap = (guiserver_bitmap*)MonAPI::MemoryMap::map(msg.arg2);
	if (this->bitmap == NULL)
	{
		printf("%s:%d:ERROR: can not get image data!\n", __FILE__, __LINE__);
		return;
	}
	
	this->width = width;
	this->height = height;
}

/**
 コンストラクタ
 @param path ファイル名
 */
Image::Image(char *path)
{
	this->width = this->height = 0;
	this->bitmap = NULL;
	
	// GUIサーバーを探す
	guisvrID = monapi_get_server_thread_id(ID_GUI_SERVER);
	if (guisvrID == THREAD_UNKNOWN) {
		printf("%s:%d:ERROR: can not connect to GUI server!\n", __FILE__, __LINE__);
		return;
	}
	
	// GUIサーバー上でビットマップをデコードする
	MessageInfo msg;
	if (MonAPI::Message::sendReceive(&msg, guisvrID, MSG_GUISERVER_DECODEIMAGE, 0, 0, 0, path)) {
		printf("%s:%d:ERROR: can not connect to GUI server!\n", __FILE__, __LINE__);
		return;
	}
	if (msg.arg2 == 0) return;
	
	// GUIサーバー上のビットマップオブジェクトを生成する
	this->bitmap = (guiserver_bitmap*)MonAPI::MemoryMap::map(msg.arg2);
	if (this->bitmap == NULL)
	{
		printf("%s:%d:ERROR: can not get image data!\n", __FILE__, __LINE__);
		return;
	}
	
	this->width = this->bitmap->Width;
	this->height = this->bitmap->Height;
}

/** デストラクタ */
Image::~Image()
{
	// ビットマップ破棄要求
	dword handle = getHandle();
	if (MonAPI::Message::send(guisvrID, MSG_GUISERVER_DISPOSEBITMAP, handle) == 0) {
		printf("%s:%d:ERROR: can not connect to GUI server!\n", __FILE__, __LINE__);
	}
}

/** ハンドルを得る */
unsigned int Image::getHandle()
{
	if (this->bitmap != NULL) {
		return this->bitmap->Handle;
	} else {
		return 0;
	}
}

/** 指定された点の色を得る */
unsigned int Image::getPixel(int x, int y)
{
	if (x < 0 || this->width <= x || y < 0 || this->height <= y) {
		return 0;
	} else {
		return bitmap->Data[x + this->width * y];
	}
}

/**
 点を打つ
 @param x 
 @param y 
 @param color 
*/
void Image::setPixel(int x, int y, unsigned int color)
{
	if (0 <= x && x < this->width && 0 <= y && y < this->height) {
		bitmap->Data[x + this->width * y] = color;
	}
}
