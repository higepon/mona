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

int regist(List<char*>* destList, MessageInfo* info);
int sendKeyInformation(KeyBoardManager* manager, List<char*>* destList, MessageInfo* info);

int main() {

    /* initilize KeyBoardManager */
    KeyBoardManager* manager = new KeyBoardManager();
    manager->init();

    /* initilize destination list */
    List<char*>* destList = new HList<char*>();

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

            default:
                /* igonore this message */
                break;
            }
        }
    }
}

int sendKeyInformation(KeyBoardManager* manager, List<char*>* destList, MessageInfo* info) {

    MessageInfo message;
    KeyInfo keyinfo;

    /* scan code to key information */
    byte scancode = info->arg1;
    manager->setKeyScanCode(scancode);
    manager->getKeyInfo(&keyinfo);

    /* create message */
    memset(&message, 0, sizeof(MessageInfo));
    message.arg1 = keyinfo.keycode;
    message.arg2 = keyinfo.modifiers;

    /* send message */
    for (int i = destList->size() - 1; i >= 0; i--) {
        if (Message::send(destList->get(i), &message)) {
            printf("send error to %s", destList->get(i));
            destList->removeAt(i);
        }
    }

    return 0;
}

int regist(List<char*>* destList, MessageInfo* info) {

    char* source = (char*)(info->arg1);
    printf("message is %s", source);
    char* target = new char[strlen(source) + 1];
    strcpy(target, source);
    destList->add(target);
    return 0;
}
