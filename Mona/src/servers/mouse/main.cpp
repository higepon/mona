/*!
  \file  MouseServer
  \brief class Mouse information

  Copyright (c) 2004 Higepon
  All rights reserved.
  License=MIT/X Licnese

  \author  HigePon
  \version $Revision$
  \date   create:2004/02/08 update:$Date$
*/

#include <monapi.h>
#include <monalibc.h>
#include <monapi/messages.h>
#include <monapi/io.h>

using namespace MonAPI;

#define INIT_PROCESS "MONITOR.BIN"

/*----------------------------------------------------------------------
    Private variables
----------------------------------------------------------------------*/
static CommonParameters* commonParams;
static Screen screen;
static const char* cursor[] =
{
    "1",
    "11",
    "121",
    "1221",
    "12221",
    "122221",
    "1222221",
    "12222221",
    "122222221",
    "1222222221",
    "12222211111",
    "1221221",
    "121 1221",
    "11  1221",
    "1    1221",
    "     1221",
    "      1221",
    "      1221",
    "       11",
    NULL
};

/*----------------------------------------------------------------------
    Cursor
----------------------------------------------------------------------*/
class Cursor
{
private:
    int width, height, sw, sh, bypp, bx, by;
    byte* buffer;

public:
    Cursor() : width(0), height(0), bx(0), by(0), buffer(NULL)
    {
        for (int i = 0; cursor[i] != NULL; i++)
        {
            int len = strlen(cursor[i]);
            if (this->width < len) this->width = len;
            this->height = i + 1;
        }
        this->sw = screen.getWidth();
        this->sh = screen.getHeight();
        this->bypp = screen.getBpp() / 8;
    }

    virtual ~Cursor()
    {
        if (this->buffer != NULL) delete [] this->buffer;
    }

    void Erase()
    {
        if (this->buffer == NULL) return;

        byte* p1 = this->buffer;
        byte* p2 = screen.getVRAM() + (this->bx + this->sw * this->by) * this->bypp;
        for (int y = 0; y < this->height; y++)
        {
            const char* data = cursor[y];
            int yy = this->by + y, len = strlen(data);
            for (int x = 0; x < this->width; x++)
            {
                int xx = this->bx + x;
                if (x < len && data[x] != ' ' && 0 <= xx && xx < this->sw && 0 <= yy && yy < this->sh)
                {
                    memcpy(p2, p1, this->bypp);
                }
                p1 += this->bypp;
                p2 += this->bypp;
            }
            p2 += (this->sw - this->width) * this->bypp;
        }
    }

public:
    void Paint(int pos_x, int pos_y)
    {
        this->bx = pos_x;
        this->by = pos_y;
        if (this->buffer == NULL) this->buffer = new byte[this->width * this->height * this->bypp];
        byte* p1 = this->buffer;
        byte* p2 = screen.getVRAM() + (this->bx + this->sw * this->by) * this->bypp;
        for (int y = 0; y < this->height; y++)
        {
            const char* data = cursor[y];
            int yy = this->by + y, len = strlen(data);
            for (int x = 0; x < this->width; x++)
            {
                int xx = this->bx + x;
                if (x < len && data[x] != ' ' && 0 <= xx && xx < this->sw && 0 <= yy && yy < this->sh)
                {
                    memcpy(p1, p2, this->bypp);
                    switch (data[x])
                    {
                        case '1':
                            memset(p2, 0xff, this->bypp);
                            break;
                        case '2':
                            memset(p2, 0, this->bypp);
                            break;
                    }
                }
                else
                {
                    memset(p1, 0, this->bypp);
                }
                p1 += this->bypp;
                p2 += this->bypp;
            }
            p2 += (this->sw - this->width) * this->bypp;
        }
    }
};

/*----------------------------------------------------------------------
    Mouse Server
----------------------------------------------------------------------*/
class Mouse
{
public:
    static const int MOUSE_TIMEOUT = 50000;

    static int init()
    {
        byte data;
        outp8(0x64, 0x20);
        if (waitReadable())
        {
            return 1;
        }
        data = inp8(0x60);

        outp8(0x64, 0x60);
        if (waitWritable())
        {
            return 2;
        }

        outp8(0x60, data & (~0x30) | 0x3);
        if (!waitReadable())
        {
            inp8(0x60);
        }

        outp8(0x64, 0xd4);
        if (waitWritable())
        {
            return 4;
        }
        outp8(0x60, 0xf4);
        if (waitReadable())
        {
            return 5;
        }
        inp8(0x60);
        return 0;
    }

    static int waitWritable()
    {
        byte status;
        int i;

        for (i = 0, status = inp8(0x64); i < MOUSE_TIMEOUT; i++, status = inp8(0x64)) {

            /* writable */
            if ((status & 0x03) == 0x00) {
                break;
            }
        }
        return (i == MOUSE_TIMEOUT) ? -1 : 0;
    }

    static int waitReadable()
    {
        byte status;
        int i;

        for (i = 0, status = inp8(0x64); i < MOUSE_TIMEOUT; i++, status = inp8(0x64)) {

            /* readable */
            if ((status & 0x01) == 0x01) {
                break;
            }
        }

        return (i == MOUSE_TIMEOUT) ? -1 : 0;
    }

    static void enable()
    {
        /* enable mouse interrupt slave unmask */
        monapi_set_irq(12, MONAPI_TRUE, MONAPI_TRUE);
    }

    static void disable()
    {
        monapi_set_irq(12, MONAPI_FALSE, MONAPI_TRUE);
    }

    static void enableKeyboard()
    {
        monapi_set_irq(1, MONAPI_TRUE, MONAPI_TRUE);
    }

    static void disableKeyboard()
    {
        monapi_set_irq(1, MONAPI_FALSE, MONAPI_TRUE);
    }
};

/*----------------------------------------------------------------------
    Mouse Server
----------------------------------------------------------------------*/
class MouseServer
{
private:
    bool needPaint;
    int disableCount;
    int posX, posY, prevX, prevY;
    char dx, dy;
    byte button, prevButton;
    int w, h;
    Screen* vscreen;
    List<dword>* destList;
    Cursor cursor;

public:
    MouseServer() : needPaint(false), disableCount(0)
    {
    }

    virtual ~MouseServer()
    {
        delete this->vscreen;
    }

public:
    bool Initialize()
    {
        MessageInfo msg;
        if (Message::sendReceive(&msg, monapi_get_server_thread_id(ID_PROCESS_SERVER), MSG_PROCESS_GET_COMMON_PARAMS) != 0)
        {
            syscall_print("Mouse Server: can not get common parameters\n");
            return false;
        }
        commonParams = (CommonParameters*)MemoryMap::map(msg.arg2);

        /* mouse information destination list */
        this->destList = new HList<dword>();
        if (this->destList == NULL)
        {
            syscall_print("Mouse Server: destList error\n");
            return false;
        }

        /* screen size */
        this->w = screen.getWidth();
        this->h = screen.getHeight();

        /* cursor to center */
        commonParams->mouse.x = this->posX = this->prevX = this->w / 2;
        commonParams->mouse.y = this->posY = this->prevY = this->h / 2;
        commonParams->mouse.buttons = this->button = this->prevButton = 0;

        /* server start ok */
        if (!SendServerOK()) return false;

        cursor.Paint(this->posX, this->posY);

        return true;
    }

    void MessageLoop()
    {
        for (MessageInfo receive;;)
        {
            if (Message::receive(&receive)) continue;

            switch(receive.header)
            {
            case MSG_MOUSE_REGIST_TO_SERVER:
                /* arg1 = tid */
                this->destList->add(receive.arg1);
                Message::reply(&receive);
                break;

            case MSG_MOUSE_UNREGIST_FROM_SERVER:
                /* arg1 = tid */
                this->destList->remove(receive.arg1);
                Message::reply(&receive);
                break;

           case MSG_MOUSE_ENABLE_CURSOR:
                if (this->disableCount > 0)
                {
                    this->disableCount--;
                    if (this->disableCount == 0) cursor.Paint(this->prevX, this->prevY);
                }
                Message::reply(&receive);
                break;

            case MSG_MOUSE_DISABLE_CURSOR:
                if (this->disableCount == 0) cursor.Erase();
                this->disableCount++;
                Message::reply(&receive);
                break;

            case MSG_MOUSE_GET_CURSOR_POSITION:
                Message::reply(&receive, this->prevX, this->prevY);
                break;

            case MSG_INTERRUPTED:
                /* we get not all data */
                if (GetMouseData() != 2) break;

                /* state not changed. */
                if (!SetCurosor()) break;

                Paint();
                SendMouseInformation();

                break;

            default:
                /* ignore */
                printf("mouse:header=%x", receive.header);
                break;
            }
        }
    }

protected:
    void Paint()
    {
        if (this->disableCount == 0)
        {
            cursor.Erase();
            cursor.Paint(this->posX, this->posY);
        }
        commonParams->mouse.x = this->prevX = this->posX;
        commonParams->mouse.y = this->prevY = this->posY;
        commonParams->mouse.buttons = this->prevButton = this->button;
    }

    bool SendServerOK()
    {
        dword targetID = Message::lookupMainThread(INIT_PROCESS);
        if (targetID == THREAD_UNKNOWN)
        {
            printf("Mouse Server: MONITER not found\n");
            return false;
        }

        if (Message::send(targetID, MSG_SERVER_START_OK))
        {
            printf("Mouse Server: server start send error\n");
            return false;
        }

        return true;
    }

    void SendMouseInformation()
    {
        MessageInfo info;
        Message::create(&info, MSG_MOUSE_INFO, this->posX, this->posY, this->button);

        for (int i = this->destList->size() - 1; i >= 0; i--)
        {
            if (Message::send(this->destList->get(i), &info))
            {
                printf("Mouse Server: send error to thread id = %x", this->destList->get(i));
                this->destList->removeAt(i);
            }
        }
    }

    int GetMouseData()
    {
        static int state = 0;

        byte data = inp8(0x60);

        if(state == 0 && !(data & 0x08)) return -1;

        switch (state & 3)
        {
        case 0:
            this->button = (data & 0x01) | (data & 0x02);
            state = 1;
            this->needPaint = false;
            break;
        case 1:
            this->dx = data;
            state = 2;
            break;
        case 2:
            this->dy = -1 * data;
            state = 0;
            this->needPaint = true;
            break;
        }

        return state & 3;
    }

    bool SetCurosor()
    {
        /* mouse info */
        this->posX += (int)(((double)dx) * 1.5);
        this->posY += (int)(((double)dy) * 1.5);

        /* mouse cursor size */
        if (this->posX >= this->w) this->posX = this->w - 1;
        if (this->posY >= this->h) this->posY = this->h - 1;
        if (this->posX < 0) this->posX = 0;
        if (this->posY < 0) this->posY = 0;

        /* mouse state changed? */
        return ((this->prevButton != this->button) || ((this->prevX != this->posX || this->prevY != this->posY)));
    }
};

/*----------------------------------------------------------------------
    Main
----------------------------------------------------------------------*/
int MonaMain(List<char*>* pekoe)
{
    /* allow I/O from user mode */
    syscall_get_io();

    Mouse::disable();
    Mouse::disableKeyboard();

    /* Mouse init */
    int mouse = Mouse::init();
    if (mouse)
    {
        printf("Mouse init error=%d\n", mouse);
        return -1;
    }

    /* Mouse server initialize */
    MouseServer server;
    if (!server.Initialize())
    {
        syscall_print("Mouse Server: initialize error\n");
        return -1;
    }

    Mouse::enable();
    Mouse::enableKeyboard();

    /* we receive MSG_INTERRUPTED from IRQ12 Handler */
    syscall_set_irq_receiver(12);

    /* service start */
    server.MessageLoop();

    return 0;
}
