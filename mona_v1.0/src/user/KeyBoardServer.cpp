/*!
  \file  KeyBoardServer
  \brief class KeyBoard information

  Copyright (c) 2004 Higepon
  All rights reserved.
  License=MIT/X Licnese

  \author  HigePon
  \version $Revision$
  \date   create:2004/01/25 update:$Date$
*/

#include <userlib.h>
#include <collection.h>
#include <KeyBoardManager.h>

int regist(List<dword>* destList, MessageInfo* info);
int sendKeyInformation(KeyBoardManager* manager, List<dword>* destList, MessageInfo* info);

void dummy() {
}

int main() {

    /* initilize KeyBoardManager */
    KeyBoardManager* manager = new KeyBoardManager();
    manager->init();

    /* initilize destination list */
    List<dword>* destList = new HList<dword>();

    int i  = 0;

    int posX = 0;
    int posY = 0;
    int x, y;
    bool yPlus;
    bool xPlus;
    bool leftClickd;
    bool rightClickd;
    Screen screen;

    /* Message loop */
    for (;;) {

        MessageInfo info;

        /* receive */
        if (!Message::receive(&info)) {

            switch(info.header) {

            case MSG_KEY_SCANCODE:

                sendKeyInformation(manager, destList, &info);
                break;

            case MSG_KEY_REGIST_TO_SERVER:

                regist(destList, &info);
                break;

            case MSG_MOUSE_1:

                {
                    byte result = (byte)(info.arg1);
                    xPlus = !(result & 0x10);
                    yPlus = !(result & 0x20);
                    leftClickd = (result & 0x01);
                    rightClickd = (result & 0x02);
                }

            case MSG_MOUSE_2:

                {
                    byte result = (byte)(info.arg1);
                    //                    x = xPlus ? (int)result : (int)(-1 * ~(result - 1));
                    x = (char)result;
                }

                break;

            case MSG_MOUSE_3:

                {
                    byte result = (byte)(info.arg1);
                    //                    y = yPlus ? (int)result : (int)(-1 * ~(result - 1));
                    y = -1 * (char)result;
                }

                posX += x;
                if (posX > screen.getXResolution()) posX = screen.getXResolution();
                if (posX < 0) posX = 0;
                posY += y;
                if (posY > screen.getYResolution()) posY = screen.getYResolution();
                if (posY < 0) posY = 0;

                if (leftClickd) {
                    screen.fillRect16(posX , posY, 10, 10, Color::rgb(0x00, 0x00, 0xFF));
                } else {
                    screen.fillRect16(posX , posY, 1, 1, Color::rgb(0xFF, 0xFF, 0xFF));
                }

                break;

            default:
                /* igonore this message */
                break;
            }
        }
    }
}

int sendKeyInformation(KeyBoardManager* manager, List<dword>* destList, MessageInfo* info) {

    MessageInfo message;
    KeyInfo keyinfo;

    /* scan code to virtual key information */
    byte scancode = info->arg1;
    manager->setKeyScanCode(scancode);
    manager->getKeyInfo(&keyinfo);

    /* create message */
    memset(&message, 0, sizeof(MessageInfo));
    Message::create(&message, MSG_KEY_VIRTUAL_CODE, keyinfo.keycode, keyinfo.modifiers, 0, NULL);

    //    printf("%x", keyinfo.keycode);

    /* send message */
    for (int i = destList->size() - 1; i >= 0; i--) {

        if (Message::send(destList->get(i), &message)) {
            printf("send error to pid = %x", destList->get(i));
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
