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

using namespace MonAPI;

typedef struct {
    int x;
    int y;
    bool leftClickd;
    bool rightClickd;
} MouseInfo;

int regist(List<dword>* destList, MessageInfo* info);
int unregist(List<dword>* destList, MessageInfo* info);
int sendMouseInformation(List<dword>* destList, MessageInfo* info);

int MonaMain(List<char*>* pekoe) {

    /* initilize destination list */
    List<dword>* destList = new HList<dword>();

    MouseInfo info;
    Screen screen;
    MessageInfo receive;
    MessageInfo send;
    byte result;
    int posX, posY, prevX, prevY;

    int xResolution = screen.getWidth();
    int yResolution = screen.getHeight();

    posX = prevX = xResolution / 2;
    posY = prevY = yResolution / 2;

    /* draw mouse cursor to virtual screen */
    VirtualScreen vscreen(1024 * 40);
    vscreen.fillRect16(0, 0, 10, 10, Color::rgb(0x00, 0xCC, 0x56));

    /* Server start ok */
    dword targetID = Message::lookupMainThread("INIT");
    if (targetID == 0xFFFFFFFF)
    {
        printf("MouseServer:INIT not found\n");
        exit(1);
    }

    /* create message */
    Message::create(&send, MSG_SERVER_START_OK, 0, 0, 0, NULL);

    /* send */
    if (Message::send(targetID, &send)) {
        printf("MouseServer:INIT error\n");
    }

    /* paint */
    Screen::bitblt(&screen, posX , posY, 3, 3, &vscreen, 0, 0, Raster::XOR);

    /* Message loop */
    for (;;) {

        /* receive */
        if (!Message::receive(&receive)) {

            switch(receive.header) {

            case MSG_MOUSE_REGIST_TO_SERVER:

                regist(destList, &receive);
                break;

            case MSG_MOUSE_UNREGIST_FROM_SERVER:

                unregist(destList, &receive);
                break;

            case MSG_MOUSE:

                result = (byte)(receive.arg1);
                info.leftClickd  = (bool)(result & 0x01);
                info.rightClickd = (bool)(result & 0x02);

                result = (byte)(receive.arg2);
                info.x = (char)result;

                result = (byte)(receive.arg3);
                info.y = -1 * (char)result;

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

                Message::create(&send, MSG_MOUSE_INFO, posX, posY
                                , (info.leftClickd ? 0x01 : 0x00) | (info.rightClickd ? 0x02 : 0x00), NULL);
                sendMouseInformation(destList, &send);

                break;

            default:

                /* igonore this message */
                break;
            }
        }
    }
    return 0;
}

int sendMouseInformation(List<dword>* destList, MessageInfo* info) {

    /* send message */
    for (int i = destList->size() - 1; i >= 0; i--) {

        if (Message::send(destList->get(i), info)) {
            printf("Mouse:send error to thread id = %x", destList->get(i));
            destList->removeAt(i);
        }
    }

    return 0;
}

int regist(List<dword>* destList, MessageInfo* info) {

    dword id = info->arg1;
    destList->add(id);
    return 0;
}

int unregist(List<dword>* destList, MessageInfo* info) {

    dword id = info->arg1;
    destList->remove(id);
    return 0;
}
