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

#include <userlib.h>
#include <collection.h>

typedef struct {
    int x;
    int y;
    bool leftClickd;
    bool rightClickd;
} MouseInfo;

int regist(List<dword>* destList, MessageInfo* info);
int sendMouseInformation(List<dword>* destList, MessageInfo* info);

int MonaMain(List<char*>* pekoe) {

    /* initilize destination list */
    List<dword>* destList = new HList<dword>();

    MouseInfo info;
    Screen screen;
    MessageInfo receive;
    MessageInfo send;
    byte result;
    int x, y;
    int posX, posY, prevX, prevY;

    int xResolution = screen.getWidth();
    int yResolution = screen.getHeight();

    /* draw mouse cursor to virtual screen */
    VirtualScreen vscreen(1024 * 40);
    vscreen.fillRect16(0, 0, 10, 10, Color::rgb(0x00, 0xCC, 0x56));

    /* Message loop */
    for (;;) {

        /* receive */
        if (!Message::receive(&receive)) {

            switch(receive.header) {

            case MSG_MOUSE_REGIST_TO_SERVER:

                regist(destList, &receive);
                break;

            case MSG_MOUSE_1:

                result = (byte)(receive.arg1);
                info.leftClickd  = (bool)(result & 0x01);
                info.rightClickd = (bool)(result & 0x02);
                break;

            case MSG_MOUSE_2:

                result = (byte)(receive.arg1);
                info.x = (char)result;
                break;

            case MSG_MOUSE_3:

                result = (byte)(receive.arg1);
                info.y = -1 * (char)result;

                Message::create(&send, MSG_MOUSE_INFO, info.x, info.y
                                , (info.leftClickd ? 0x01 : 0x00) | (info.rightClickd ? 0x02 : 0x00), NULL);
                sendMouseInformation(destList, &send);

                posX += info.x;
                if (posX > xResolution) posX = xResolution;
                if (posX < 0) posX = 0;
                posY += info.y;
                if (posY > yResolution) posY = yResolution;
                if (posY < 0) posY = 0;

                Screen::bitblt(&screen, prevX, prevY, 3, 3, &vscreen, 0, 0, Raster::XOR);
                Screen::bitblt(&screen, posX , posY, 3, 3, &vscreen, 0, 0, Raster::XOR);

                prevX = posX;
                prevY = posY;

                break;

            default:
                /* igonore this message */
                break;
            }
        }
    }
}

int sendMouseInformation(List<dword>* destList, MessageInfo* info) {

    /* send message */
    for (int i = destList->size() - 1; i >= 0; i--) {

        if (Message::send(destList->get(i), info)) {
            printf("Mouse:send error to pid = %x", destList->get(i));
            destList->removeAt(i);
        }
    }

    return 0;
}

int regist(List<dword>* destList, MessageInfo* info) {

    dword pid = info->arg1;
    destList->add(pid);
    return 0;
}
