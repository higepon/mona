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

#define INIT_PROCESS "INIT"

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
    void SendMouseInformation(MessageInfo* info);
    void PaintCursor(int x, int y);

private:
    bool needPaint;
    int disableCount;
    int posX;
    int posY;
    int prevX;
    int prevY;
    int w;
    int h;
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
    /* mouse information destination list */
    this->destList = new HList<dword>();

    if (this->destList == NULL)
    {
        printf("MouseServer:destList error\n");
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
    this->vscreen->fillRect16(0, 0, 10, 10, Color::rgb(0x00, 0xCC, 0x56));

    /* cursor to center */
    this->posX = this->prevX = this->w / 2;
    this->posY = this->prevY = this->h / 2;

    /* server start ok */
    if (!SendServerOK()) return false;

    PaintCursor(this->posX, this->posY);

    return true;
}

void MouseServer::MessageLoop()
{
    MessageInfo receive;
    MessageInfo send;

    for (;;)
    {
        if (!Message::receive(&receive))
        {
            switch(receive.header)
            {
            case MSG_MOUSE_REGIST_TO_SERVER:

                /* arg1 = tid */
                this->destList->add(receive.arg1);
                break;

            case MSG_MOUSE_UNREGIST_FROM_SERVER:

                /* arg1 = tid */
                this->destList->remove(receive.arg1);
                break;

           case MSG_MOUSE_ENABLE_CURSOR:

                if (this->disableCount > 0)
                {
                    disableCount--;
                    PaintCursor(posX, posY);
                }

                Message::create(&send, MSG_RESULT_OK, receive.header);
                Message::send(receive.from, &send);

                break;

            case MSG_MOUSE_DISABLE_CURSOR:

                PaintCursor(posX, posY);
                disableCount++;

                Message::create(&send, MSG_RESULT_OK, receive.header);
                Message::send(receive.from, &send);

                break;

            case MSG_MOUSE:

                {
                    byte result    = (byte)(receive.arg1);
                    byte clickInfo = (result & 0x01) | (result & 0x02);

                    char x = (byte)(receive.arg2);
                    char y = (byte)(receive.arg3) * (-1);

                    this->posX += x;
                    this->posY += y;

                    if (this->posX > this->w) this->posX = this->w;
                    if (this->posY > this->h) this->posY = this->h;
                    if (this->posX < 0) this->posX = 0;
                    if (this->posY < 0) this->posY = 0;

                    Paint();

                    Message::create(&send, MSG_MOUSE_INFO, posX, posY, clickInfo);
                    SendMouseInformation(&send);
                }

                break;

            default:

                /* ignore */
                break;
            }
        }
    }
}

void MouseServer::Paint()
{
    PaintCursor(this->prevX, this->prevY);
    PaintCursor(this->posX , this->posY);
    prevX = posX;
    prevY = posY;
}

void MouseServer::SendMouseInformation(MessageInfo* info)
{
    for (int i = this->destList->size() - 1; i >= 0; i--)
    {
        if (Message::send(this->destList->get(i), info))
        {
            printf("Mouse:send error to thread id = %x", this->destList->get(i));
            this->destList->removeAt(i);
        }
    }
}

bool MouseServer::SendServerOK()
{
    dword targetID = Message::lookupMainThread(INIT_PROCESS);
    if (targetID == 0xFFFFFFFF)
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
    Screen::bitblt(screen, x, y, 3, 3, vscreen, 0, 0, Raster::XOR);
}


static MouseServer* server;

int MonaMain(List<char*>* pekoe)
{
    server = new MouseServer();

    if (!server->Initialize())
    {
        printf("MouseServer:initialize error\n");
        return -1;
    }

    server->MessageLoop();

    return 0;
}
