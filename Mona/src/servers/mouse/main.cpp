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

using namespace MonAPI;

#define INIT_PROCESS "MONITOR.BIN"

/*----------------------------------------------------------------------
    Mouse Server
----------------------------------------------------------------------*/
class Mouse
{
  public:
    static int init();
    static int waitWritable();
    static int waitReadable();
    static void enable();
    static void disable();
    static void enableKeyboard();
    static void disableKeyboard();

  public:
    static const int MOUSE_TIMEOUT = 50000;
};

/*----------------------------------------------------------------------
    Mouse Server
----------------------------------------------------------------------*/
class MouseServer
{
public:
    MouseServer();
    virtual ~MouseServer();

public:
    bool Initialize();
    void MessageLoop();

protected:
    void Paint();
    bool SendServerOK();
    void SendMouseInformation();
    void PaintCursor(int x, int y);
    int GetMouseData();
    bool SetCurosor();

private:
    bool needPaint;
    int disableCount;
    int posX, posY, prevX, prevY;
    char dx, dy;
    byte button, prevButton;
    int w, h;
    Screen* screen;
    Screen* vscreen;
    List<dword>* destList;
};

/*----------------------------------------------------------------------
    Private variables
----------------------------------------------------------------------*/
static CommonParameters* commonParams;
static MouseServer* server;

/*----------------------------------------------------------------------
    Mouse
----------------------------------------------------------------------*/
int Mouse::init()
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

int Mouse::waitWritable() {

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

int Mouse::waitReadable() {

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

void Mouse::enable()
{

    /* enable mouse interrupt slave unmask */
    outp8(0x21, (inp8(0x21) & 0xFB)); /* IR2 cascade */
    outp8(0xA1, (inp8(0xA1) & 0xEF)); /* IR4         */
}

void Mouse::disable()
{
    outp8(0xA1, inp8(0xA1) | 0x10);
}

void Mouse::disableKeyboard()
{
    outp8(0x21, inp8(0x21) | 0x02);
}

void Mouse::enableKeyboard()
{
    outp8(0x21, inp8(0x21) & 0xFD);
}

/*----------------------------------------------------------------------
    MouseServer
----------------------------------------------------------------------*/
MouseServer::MouseServer() : needPaint(false), disableCount(0)
{
}

MouseServer::~MouseServer()
{
    delete this->screen;
    delete this->vscreen;
}

bool MouseServer::Initialize()
{
    MessageInfo msg;

    if (Message::sendReceive(&msg, monapi_get_server_thread_id(ID_PROCESS_SERVER), MSG_PROCESS_GET_COMMON_PARAMS) != 0)
    {
        syscall_print("MouseServer: can not get common parameters\n");
        return false;
    }

    commonParams = (CommonParameters*)MemoryMap::map(msg.arg2);

    /* mouse information destination list */
    this->destList = new HList<dword>();

    if (this->destList == NULL)
    {
        syscall_print("MouseServer: destList error\n");
        return false;
    }

    /* create screen */
    this->screen  = new Screen();
    this->vscreen = new VirtualScreen(40 * 1024);

    if (this->screen == NULL || this->vscreen == NULL)
    {
        printf("MouseServer: screen error\n");
        return false;
    }

    /* screen size */
    this->w = screen->getWidth();
    this->h = screen->getHeight();

    /* draw mouse cursor to virtual screen */
    for (int i = 0; i < 5; i++)
    {
        this->vscreen->fillRect16(0, i, i + 1, 1, Color::rgb(0x00, 0xCC, 0x56));
    }

    for (int i = 0; i < 2; i++)
    {
        this->vscreen->fillRect16(0, i + 5, 2 - i, 1, Color::rgb(0x00, 0xCC, 0x56));
    }

    /* cursor to center */
    commonParams->mouse.x = this->posX = this->prevX = this->w / 2;
    commonParams->mouse.y = this->posY = this->prevY = this->h / 2;
    commonParams->mouse.buttons = this->button = this->prevButton = 0;

    /* server start ok */
    if (!SendServerOK()) return false;

    PaintCursor(this->posX, this->posY);

    return true;
}

void MouseServer::MessageLoop()
{
    MessageInfo receive;

    for (;;)
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
                disableCount--;
                PaintCursor(this->prevX, this->prevY);
            }

            Message::reply(&receive);

            break;

        case MSG_MOUSE_DISABLE_CURSOR:

            PaintCursor(this->prevX, this->prevY);
            disableCount++;

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

bool MouseServer::SetCurosor()
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

int MouseServer::GetMouseData()
{
    static int state = 0;

    byte data = inp8(0x60);

    if(state == 0 && !(data & 0x08)) return -1;

    switch(state & 3)
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

void MouseServer::Paint()
{
    PaintCursor(this->prevX, this->prevY);
    PaintCursor(this->posX , this->posY);
    commonParams->mouse.x = this->prevX = this->posX;
    commonParams->mouse.y = this->prevY = this->posY;
    commonParams->mouse.buttons = this->prevButton = this->button;
}

void MouseServer::SendMouseInformation()
{
    MessageInfo info;
    Message::create(&info, MSG_MOUSE_INFO, this->posX, this->posY, this->button);

    for (int i = this->destList->size() - 1; i >= 0; i--)
    {
        if (Message::send(this->destList->get(i), &info))
        {
            printf("Mouse:send error to thread id = %x", this->destList->get(i));
            this->destList->removeAt(i);
        }
    }
}

bool MouseServer::SendServerOK()
{
    dword targetID = Message::lookupMainThread(INIT_PROCESS);
    if (targetID == THREAD_UNKNOWN)
    {
        printf("MouseServer:MONITER not found\n");
        return false;
    }

    if (Message::send(targetID, MSG_SERVER_START_OK))
    {
        printf("MouseServer:server start send error\n");
        return false;
    }

    return true;
}

void MouseServer::PaintCursor(int x, int y)
{
    if (this->disableCount > 0) return;
    Screen::bitblt(screen, x, y, 5, 7, vscreen, 0, 0, Raster::XOR);
}

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
    server = new MouseServer();
    if (!server->Initialize())
    {
        syscall_print("MouseServer: initialize error\n");
        return -1;
    }

    Mouse::enable();
    Mouse::enableKeyboard();

    /* we receive MSG_INTERRUPTED from IRQ12 Handler */
    syscall_set_irq_receiver(12);



    /* service start */
    server->MessageLoop();
    return 0;
}
