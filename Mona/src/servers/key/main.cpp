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

#include <monapi.h>
#include <monapi/messages.h>
#include <monapi/io.h>
#include "KeyboardManager.h"

using namespace MonAPI;

int regist(List<dword>* destList, MessageInfo* info);
int unregist(List<dword>* destList, MessageInfo* info);
int sendKeyInformation(KeyBoardManager* manager, List<dword>* destList, byte scancode);


bool WaitInterruptWithTimeout(dword ms, byte irq, const char* file = "no file", int line = 0);


#define WAIT_INTERRUPT(ms, irq) WaitInterruptWithTimeout(ms, irq, __FILE__, __LINE__)

bool WaitInterruptWithTimeout(dword ms, byte irq, const char* file, int line)
{
    MessageInfo msg;

    dword timerId = set_timer(ms);

    for (int i = 0; ; i++)
    {
        int result = Message::peek(&msg, i);

        if (result != 0)
        {
            i--;
            syscall_mthread_yield_message();
        }
        else if (msg.header == MSG_TIMER)
        {
            if (msg.arg1 != timerId) continue;
            kill_timer(timerId);

            Message::peek(&msg, i, PEEK_REMOVE);

            printf("interrupt timeout %s:%d\n", file, line);
            return false;
        }
        else if (msg.header == MSG_INTERRUPTED)
        {
            if (msg.arg1 != irq) continue;
            kill_timer(timerId);

            Message::peek(&msg, i, PEEK_REMOVE);
            return true;
        }
    }
    return false;
}

int MonaMain(List<char*>* pekoe)
{
    /* user mode I/O */
    syscall_get_io();

    /* initilize KeyBoardManager */
    KeyBoardManager* manager = new KeyBoardManager();
    manager->init();

    /* initilize destination list */
    List<dword>* destList = new HList<dword>();
    MessageInfo info;

    /* Server start ok */
    dword targetID = Message::lookupMainThread("MONITOR.BIN");
    if (targetID == 0xFFFFFFFF)
    {
        printf("KeyBoardServer:MONITOR not found\n");
        exit(1);
    }

    /* create message */
    Message::create(&info, MSG_SERVER_START_OK, 0, 0, 0, NULL);

    /* send */
    if (Message::send(targetID, &info)) {
        printf("KeyBoardServer:INIT error\n");
    }

    syscall_set_irq_receiver(1);


    /* Message loop */
    for (;;)
    {
        /* receive */
        if (!Message::receive(&info))
        {
            switch(info.header)
            {
            case MSG_INTERRUPTED:

                sendKeyInformation(manager, destList, inp8(0x60));

                break;

            case MSG_KEY_REGIST_TO_SERVER:

                regist(destList, &info);
                Message::reply(&info);
                break;

            case MSG_KEY_UNREGIST_FROM_SERVER:

                unregist(destList, &info);
                Message::reply(&info);
                break;

            case MSG_MEMORY_MAP_ID:

                {
                    dword id = info.arg1;
                    char* p = (char*)(MemoryMap::map(id));
                    printf("[share!]%s\n", p);
                    MemoryMap::unmap(id);

                }
                break;

            default:
                /* igonore this message */

                break;
            }
        }
    }
    return 0;
}

int sendKeyInformation(KeyBoardManager* manager, List<dword>* destList, byte scancode)
{
    MessageInfo message;
    KeyInfo keyinfo;

    /* scan code to virtual key information */
    if(manager->setKeyScanCode(scancode) == 0) return 0;
    manager->getKeyInfo(&keyinfo);

    /* create message */
    memset(&message, 0, sizeof(MessageInfo));
    Message::create(&message, MSG_KEY_VIRTUAL_CODE, keyinfo.keycode, keyinfo.modifiers, keyinfo.charcode, NULL);

    /* send message */
    for (int i = destList->size() - 1; i >= 0; i--)
    {
        if (Message::send(destList->get(i), &message))
        {
            printf("send error to pid = %x", destList->get(i));
            destList->removeAt(i);
        }
    }
    return 0;
}

int regist(List<dword>* destList, MessageInfo* info)
{
    dword id = info->arg1;
    destList->add(id);
    return 0;
}

int unregist(List<dword>* destList, MessageInfo* info)
{
    dword id = info->arg1;
    destList->remove(id);
    return 0;
}
