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
#include <monapi/messages.h>

#define INIT_PROCESS "INIT"

static CommonParameters* commonParams;

using namespace MonAPI;

class MouseServer
{
public:
    MouseServer();
    virtual ~MouseServer();

public:
    bool Initialize();
    void MessageLoop();
    void Paint();

private:
    bool SendServerOK();
    void SendMouseInformation();
    void PaintCursor(int x, int y);

private:
    bool needPaint;
    int disableCount;
    int posX, posY, prevX, prevY;
    byte button, prevButton;
    int w, h;
    Screen* screen;
    Screen* vscreen;
    List<dword>* destList;
};

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
        printf("MouseServer: can not get common parameters\n");
        return false;
    }
    commonParams = (CommonParameters*)MemoryMap::map(msg.arg2);

    /* mouse information destination list */
    this->destList = new HList<dword>();

    if (this->destList == NULL)
    {
        printf("MouseServer: destList error\n");
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
    //this->vscreen->fillRect16(0, 0, 10, 10, Color::rgb(0x00, 0xCC, 0x56));
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
        if (Message::receive(&receive) != 0) continue;

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

        case MSG_MOUSE:

            {
                byte result  = (byte)(receive.arg1);
                this->button = (result & 0x01) | (result & 0x02);

                char x = (byte)(receive.arg2);
                char y = (byte)(receive.arg3) * (-1);

                this->posX += (int)(((double)x) * 1.5);
                this->posY += (int)(((double)y) * 1.5);

                /* mouse cursor size */
                if (this->posX >= this->w) this->posX = this->w - 1;
                if (this->posY >= this->h) this->posY = this->h - 1;
                if (this->posX < 0) this->posX = 0;
                if (this->posY < 0) this->posY = 0;
            }

            break;

        case MSG_MOUSE_GET_CURSOR_POSITION:

            Message::reply(&receive, this->prevX, this->prevY);
            break;

        default:

            /* ignore */
            break;
        }

        if (this->prevButton != this->button
            || ((this->prevX != this->posX || this->prevY != this->posY) && !Message::exist()))
        {
            this->Paint();
            this->SendMouseInformation();
        }
    }
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
        printf("MouseServer:INIT not found\n");
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


static MouseServer* server;

int MonaMain(List<char*>* pekoe)
{
    server = new MouseServer();

    if (!server->Initialize())
    {
        printf("MouseServer: initialize error\n");
        return -1;
    }

    server->MessageLoop();

    return 0;
}
