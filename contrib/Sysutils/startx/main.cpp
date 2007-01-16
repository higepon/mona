/*
Copyright (c) 2006 bayside, Tino

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

#include <monapi.h>
#include <monapi/messages.h>
#include <jpegls.h>

using namespace MonAPI;

#define SELECT_WALLPAPER      0
#define SELECT_CUI_0          1
#define SELECT_GUI_1          2
#define SELECT_GUI_2          3

#define IMAGE_WALLPAPER       0
#define IMAGE_CUI_0_DISABLE   1
#define IMAGE_GUI_1_DISABLE   2
#define IMAGE_GUI_2_DISABLE   3
#define IMAGE_CUI_0_ENABLE    4
#define IMAGE_GUI_1_ENABLE    5
#define IMAGE_GUI_2_ENABLE    6
#define IMAGE_MAX             7

#define IMAGE_WIDTH           149
#define IMAGE_HEIGHT          112
#define IMAGE_PADDING         16

#define BOX_WIDTH             (IMAGE_WIDTH  * 3 + IMAGE_PADDING * 4)
#define BOX_HEIGHT            (IMAGE_HEIGHT + IMAGE_PADDING * 2)
#define BOX_LEFT              ((800 - BOX_WIDTH)  / 2)
#define BOX_TOP               ((600 - BOX_HEIGHT) / 2)

// スクリーンオブジェクト
Screen screen;

// イメージクラス
class Image
{
public:
    int w, h;
    uint8_t* data;
};

// ログインクラス
class StartX
{
private:
    // イメージオブジェクト配列
    Image** images;
    // 選択位置
    int selected;

public:
    StartX()
    {
        this->images = new Image*[IMAGE_MAX];
        this->images[IMAGE_WALLPAPER ]    = openFile("/APPS/STARTX.APP/MONAHERE.JPG");
        this->images[IMAGE_CUI_0_DISABLE] = openFile("/APPS/STARTX.APP/LOGOC0.JPG");
        this->images[IMAGE_GUI_1_DISABLE] = openFile("/APPS/STARTX.APP/LOGOB0.JPG");
        this->images[IMAGE_GUI_2_DISABLE] = openFile("/APPS/STARTX.APP/LOGOM0.JPG");
        this->images[IMAGE_CUI_0_ENABLE]  = openFile("/APPS/STARTX.APP/LOGOC1.JPG");
        this->images[IMAGE_GUI_1_ENABLE]  = openFile("/APPS/STARTX.APP/LOGOB1.JPG");
        this->images[IMAGE_GUI_2_ENABLE]  = openFile("/APPS/STARTX.APP/LOGOM1.JPG");
    }

    ~StartX()
    {
        for (int i = 0; i < IMAGE_MAX; i++)
        {
            delete this->images[i]->data;
            delete this->images[i];
        }
        delete[] this->images;
    }

    const char* getCommand()
    {
        // マウスカーソルを消して、壁紙を描画して、マウスカーソルを出す
        Message::send(monapi_get_server_thread_id(ID_MOUSE_SERVER), MSG_MOUSE_SET_CURSOR_POSITION, 0, 0, 0, NULL);
        monapi_call_mouse_set_cursor(0);
        this->select(SELECT_WALLPAPER);
        monapi_call_mouse_set_cursor(1);

        // マウスサーバーに登録する
        if (!monapi_register_to_server(ID_MOUSE_SERVER, MONAPI_TRUE)) exit(1);

        // キーボードサーバーに登録する
        if (!monapi_register_to_server(ID_KEYBOARD_SERVER, MONAPI_TRUE)) exit(1);

        // メッセージループ
        for (MessageInfo info;;)
        {
            if (MonAPI::Message::receive(&info) != 0) continue;

            // マウスメッセージ
            if (info.header == MSG_MOUSE_INFO)
            {
                monapi_call_mouse_set_cursor(0);

                int mx = info.arg1;
                int my = info.arg2;

                if ( (BOX_TOP + IMAGE_PADDING) < my &&
                        my < (BOX_TOP + IMAGE_HEIGHT + IMAGE_PADDING) )
                {
                    // CUI_0
                    if ( (BOX_LEFT + IMAGE_PADDING) < mx &&
                            mx < (BOX_LEFT + IMAGE_WIDTH + IMAGE_PADDING) )
                    {
                        if (this->selected != SELECT_CUI_0)
                        {
                            this->select(SELECT_CUI_0);
                        }
                        if (info.arg3 != 0)
                        {
                            break;
                        }
                    }

                    // GUI_1
                    if ( (BOX_LEFT + IMAGE_WIDTH + IMAGE_PADDING * 2) < mx &&
                            mx < (BOX_LEFT + IMAGE_WIDTH * 2 + IMAGE_PADDING * 2) )
                    {
                        if (this->selected != SELECT_GUI_1)
                        {
                            this->select(SELECT_GUI_1);
                        }
                        if (info.arg3 != 0)
                        {
                            break;
                        }
                    }

                    // GUI_2
                    if ( (BOX_LEFT + IMAGE_WIDTH * 2 + IMAGE_PADDING * 3) < mx &&
                            mx < (BOX_LEFT + IMAGE_WIDTH * 3 + IMAGE_PADDING * 3) )
                    {
                        if (this->selected != SELECT_GUI_2)
                        {
                            this->select(SELECT_GUI_2);
                        }
                        if (info.arg3 != 0)
                        {
                            break;
                        }
                    }
                }

                monapi_call_mouse_set_cursor(1);
            }
            // キーメッセージ
            else if (info.header == MSG_KEY_VIRTUAL_CODE)
            {
                int keycode = info.arg1;
                int modcode = info.arg2;

                if ((modcode & KEY_MODIFIER_DOWN) == KEY_MODIFIER_DOWN)
                {
                    monapi_call_mouse_set_cursor(0);
                    
                    // 選択した項目を実行
                    if (keycode == MonAPI::Keys::Enter && this->selected != 0)
                    {
                        break;
                    }
                    // 次の項目を選択
                    else if (keycode == MonAPI::Keys::Tab || keycode == MonAPI::Keys::Right || keycode == 102)
                    {
                        if (this->selected == SELECT_CUI_0)
                        {
                            this->select(SELECT_GUI_1);
                        }
                        else if (this->selected == SELECT_GUI_1)
                        {
                            this->select(SELECT_GUI_2);
                        }
                        else if (this->selected == SELECT_GUI_2)
                        {
                            this->select(SELECT_CUI_0);
                        }
                        else
                        {
                            this->select(SELECT_CUI_0);
                        }
                    }
                    // 前の項目を選択
                    else if (keycode == MonAPI::Keys::Left || keycode == 100)
                    {
                        if (this->selected == SELECT_CUI_0)
                        {
                            this->select(SELECT_GUI_2);
                        }
                        else if (this->selected == SELECT_GUI_1)
                        {
                            this->select(SELECT_CUI_0);
                        }
                        else if (this->selected == SELECT_GUI_2)
                        {
                            this->select(SELECT_GUI_1);
                        }
                        else
                        {
                            this->select(SELECT_GUI_2);
                        }
                    }
                    
                    monapi_call_mouse_set_cursor(1);
                }
            }
        }

        // 選択位置を取得する
        int selected = this->selected;

        // マウスサーバーから登録解除する
        monapi_register_to_server(ID_MOUSE_SERVER, MONAPI_FALSE);

        // キーボードから登録解除する
        monapi_register_to_server(ID_KEYBOARD_SERVER, MONAPI_FALSE);

        // 画面クリア
        syscall_clear_screen();
        syscall_set_cursor(0,0);
        monapi_call_mouse_set_cursor(1);

        // GUI(1) 起動
        if (selected == SELECT_GUI_1)
        {
            return "/SERVERS/GUI.EX5 /APPS/BAYGUI.INI";
        }
        // GUI(2) 起動
        else if (selected == SELECT_GUI_2)
        {
            return "/SERVERS/GUI.EX5 /APPS/MONAFRMS.INI";
        }
        // CUI(0) 起動
        else
        {
            return NULL;
        }
    }

private:
    Image* openFile(const char* filename)
    {
        // ファイルを開く
        monapi_cmemoryinfo* mi = NULL;
        mi = monapi_file_read_all(filename);
        if (mi == NULL)
        {
            return NULL;
        }

        // JPEGデコーダ初期化
        CJPEGLS* jpeg = new CJPEGLS();
        if (jpeg->Open(mi->Data, mi->Size) != 0)
        {
            monapi_cmemoryinfo_dispose(mi);
            monapi_cmemoryinfo_delete(mi);
            return NULL;
        }

        // JPEGデコード開始
        int w, h;
        jpeg->GetInfo(&w, &h);
        uint8_t* data = new uint8_t[w * h * 3];
        jpeg->Decode(data);

        // イメージオブジェクト初期化
        Image* image = new Image();
        image->w     = w;
        image->h     = h;
        image->data  = data;

        // 共有メモリ開放
        delete jpeg;
        monapi_cmemoryinfo_dispose(mi);
        monapi_cmemoryinfo_delete(mi);

        return image;
    }

    void drawImage(Image* image, int x0, int y0)
    {
        int w          = image->w;
        int h          = image->h;
        uint8_t* data     = image->data;
        int vesaWidth  = screen.getWidth();
        int vesaHeight = screen.getHeight();
        int vesaBpp    = screen.getBpp() / 8;
        int ww         = w < vesaWidth  ? w : vesaWidth;
        int hh         = h < vesaHeight ? h : vesaHeight;
        uint8_t* vesaVram = screen.getVRAM();

        for (int y = 0; y < hh; y++)
        {
            for (int x = 0; x < ww; x++)
            {
                int k  = ((x + x0) + ((y + y0) * vesaWidth)) * vesaBpp;
                int k2 = (x + (y * w)) * 3;

                uint8_t r0 = data[k2];
                uint8_t g0 = data[k2 + 1];
                uint8_t b0 = data[k2 + 2];

                // 中央の長方形をアルファブレンドして描く
                uint8_t r = r0, g = g0, b = b0;
                if (x0 == 0 &&
                        y0 == 0 &&
                        BOX_TOP < y &&
                        y < (BOX_TOP  + BOX_HEIGHT) &&
                        BOX_LEFT < x &&
                        x < (BOX_LEFT + BOX_WIDTH))
                {
                    r = (uint8_t)(r0 * 0.6);
                    g = (uint8_t)(g0 * 0.6);
                    b = (uint8_t)(b0 * 0.6);
                }

                if (vesaBpp == 2)
                {
                    *(uint16_t*)&vesaVram[k] = Color::bpp24to565(r, g, b);
                }
                else
                {
                    vesaVram[k]   = b;
                    vesaVram[k+1] = g;
                    vesaVram[k+2] = r;
                }
            }
        }
    }

    void select(int selected)
    {
        this->selected = selected;

        if (selected == 0)
        {
            this->drawImage(this->images[0], 0, 0);
        }

        Image* cui_0 = this->images[IMAGE_CUI_0_DISABLE];
        Image* gui_1 = this->images[IMAGE_GUI_1_DISABLE];
        Image* gui_2 = this->images[IMAGE_GUI_2_DISABLE];

        if (selected == SELECT_CUI_0)
        {
            cui_0 = this->images[IMAGE_CUI_0_ENABLE];
        }

        if (selected == SELECT_GUI_1)
        {
            gui_1 = this->images[IMAGE_GUI_1_ENABLE];
        }

        if (selected == SELECT_GUI_2)
        {
            gui_2 = this->images[IMAGE_GUI_2_ENABLE];
        }

        this->drawImage(cui_0, BOX_LEFT + IMAGE_PADDING, BOX_TOP + IMAGE_PADDING);
        this->drawImage(gui_1, BOX_LEFT + IMAGE_WIDTH + IMAGE_PADDING * 2, BOX_TOP + IMAGE_PADDING);
        this->drawImage(gui_2, BOX_LEFT + IMAGE_WIDTH * 2 + IMAGE_PADDING * 3, BOX_TOP + IMAGE_PADDING);
    }
};


// エントリーポイント
int main(int argc, char* argv[])
{
    // 16bpp 以下では非対応
    if (screen.getBpp() < 16)
    {
        return 0;
    }
    else
    {
        printf("Mona GUI Selector Loading .....\n");
    }

    // プロセスサーバーに登録する
    if (!monapi_register_to_server(ID_PROCESS_SERVER, 1)) exit(1);

    // ログインオブジェクト初期化
    StartX* startx = new StartX();

    // メッセージループ
    const char* args;
    while ((args = startx->getCommand()) != NULL)
    {
        uint32_t child_id;
        int result = monapi_call_process_execute_file_get_tid(args, MONAPI_FALSE, &child_id, syscall_get_tid(), NULL);
        if (result != 0) break;
        for (MessageInfo info;;)
        {
            if (MonAPI::Message::receive(&info) != 0) continue;
            if (info.header == MSG_PROCESS_TERMINATED && info.arg1 == child_id) break;
        }
    }

    // ログインオブジェクト破棄
    delete startx;

    // プロセスサーバーから登録解除する
    monapi_register_to_server(ID_PROCESS_SERVER, MONAPI_FALSE);

    return 0;
}
