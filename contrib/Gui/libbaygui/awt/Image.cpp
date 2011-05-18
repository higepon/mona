/*
Copyright (c) 2005 bayside

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "baygui.h"

namespace baygui {
    Image::Image(int width, int height)
    {
        this->width = this->height = 0;
        this->bitmap = NULL;

        // GUIサーバーを探す
        if (monapi_name_whereis("/servers/gui", guisvrID) != M_OK) {
            monapi_fatal("server not found");
        }

        // GUIサーバー上にビットマップを生成する
        MessageInfo msg;
        if (MonAPI::Message::sendReceive(&msg, this->guisvrID, MSG_GUISERVER_CREATEBITMAP, width, height, Color::lightGray))
        {
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

    #ifdef SDL
        this->bitmap->Data = new dword [width * height];
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                this->bitmap->Data[j + i * width] = Color::lightGray;
            }
        }
    #endif

        this->width = width;
        this->height = height;
    }

    Image::Image(const String& path)
    {
        initFromFilePath(path.getBytes());
    }

    Image::~Image()
    {
        disposeImage();
    }

    void Image::resize(int h, int w)
    {
        if (height == 0 || width == 0) {
            return;
        }
        MessageInfo msg;
        if (MonAPI::Message::sendReceive(&msg, this->guisvrID, MSG_GUISERVER_CREATEBITMAP, w, h, Color::lightGray)) {
            printf("%s:%d:ERROR: can not connect to GUI server!\n", __FILE__, __LINE__);
            return;
        }
        if (msg.arg2 == 0) return;

        // GUIサーバー上のビットマップオブジェクトを生成する
        guiserver_bitmap* b = (guiserver_bitmap*)MonAPI::MemoryMap::map(msg.arg2);
        if (b == NULL) {
            printf("%s:%d:ERROR: can not get image data!\n", __FILE__, __LINE__);
            return;
        }

        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                int x = (int)(((double)height/(double)h)*i);
                int y = (int)(((double)width/(double)w)*j);
                b->Data[i + w * j] = getPixel(x, y);
            }
        }

        this->width = w;
        this->height = h;
        disposeImage();
        this->bitmap = b;
    }

    void Image::disposeImage()
    {
        if (this->bitmap) {
            if (MonAPI::Message::send(this->guisvrID, MSG_GUISERVER_DISPOSEBITMAP, getHandle())) {
                printf("%s:%d:error: can not connect to gui server!\n", __FILE__, __LINE__);
            }
            this->bitmap = NULL;
        }
    }

    void Image::initFromFilePath(const char* path)
    {
        this->width = this->height = 0;
        this->bitmap = NULL;

        // GUIサーバーを探す
        if (monapi_name_whereis("/servers/gui", guisvrID) != M_OK) {
            monapi_fatal("server not found");
        }

        // GUIサーバー上でビットマップをデコードする
        MessageInfo msg;
        if (MonAPI::Message::sendReceive(&msg, this->guisvrID, MSG_GUISERVER_DECODEIMAGE, 0, 0, 0, path)) {
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

}
