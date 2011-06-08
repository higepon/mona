/*!
  \file  KeyBoardServer
  \brief class KeyBoard information

  Copyright (c) 2004 Higepon
  All rights reserved.
  License=MIT/X License

  \author  Higepon
  \version $Revision$
  \date   create:2004/01/25 update:$Date$
*/

#include <monapi.h>
#include <monapi/messages.h>
#include <monapi/io.h>
#include "KeyboardManager.h"

using namespace MonAPI;

int Register(List<uint32_t>* destList, MessageInfo* info);
int unregister(List<uint32_t>* destList, MessageInfo* info);
int sendKeyInformation(KeyBoardManager& manager, HList<uint32_t>& destList, uint8_t scancode);

static void sendToClients(HList<uint32_t>& destList, MessageInfo* msg)
{
    for (int i = destList.size() - 1; i >= 0; i--)
    {
        if (Message::send(destList.get(i), msg) != M_OK)
        {
            monapi_warn("send error to pid = %x", destList.get(i));
            uint32_t temp;
            destList.removeAt(i, &temp);
        }
    }
}

static void messageLoop(KeyBoardManager& manager)
{
    /* initilize destination list */
    HList<uint32_t> destList;
    MessageInfo info;

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

            case MSG_KEY_PRESS:
            {
                MessageInfo message;
                Message::create(&message, MSG_KEY_VIRTUAL_CODE, info.arg1, KEY_MODIFIER_DOWN, info.arg1, NULL);
                sendToClients(destList, &message);
                Message::reply(&info);
                break;
            }
            case MSG_KEY_RELEASE:
            {
                MessageInfo message;
                Message::create(&message, MSG_KEY_VIRTUAL_CODE, info.arg1, KEY_MODIFIER_UP, info.arg1, NULL);
                sendToClients(destList, &message);
                Message::reply(&info);
                break;
            }
            case MSG_ADD:

                Register(&destList, &info);
                Message::reply(&info);
                break;

            case MSG_REMOVE:

                unregister(&destList, &info);
                Message::reply(&info);
                break;

            default:
                /* igonore this message */

                break;
            }
        }
    }
}

int main(int argc, char* argv[])
{
    /* user mode I/O */
    syscall_get_io();

    const char* MAP_FILE_PATH = "/SERVERS/KEYBDMNG.map";
    uint32_t pid = syscall_get_pid();
    intptr_t ret = syscall_stack_trace_enable(pid, MAP_FILE_PATH);
    if (ret != M_OK) {
        monapi_warn("syscall_stack_trace_enable error %d\n", ret);
        exit(-1);
    }

    /* initilize KeyBoardManager */
    KeyBoardManager manager;
    manager.init();

    if (monapi_notify_server_start("MONITOR.BIN") != M_OK)
    {
        exit(-1);
    }

    syscall_set_irq_receiver(1, 0);

    if (monapi_name_add("/servers/keyboard") != M_OK) {
        monapi_fatal("monapi_name_add failed");
    }
    messageLoop(manager);
    return 0;
}

int sendKeyInformation(KeyBoardManager& manager, HList<uint32_t>& destList, uint8_t scancode)
{
    MessageInfo message;
    KeyInfo keyinfo;

    /* scan code to virtual key information */
    if(manager.setKeyScanCode(scancode) == 0) return 0;
    manager.getKeyInfo(&keyinfo);

    /* create message */
    memset(&message, 0, sizeof(MessageInfo));
    Message::create(&message, MSG_KEY_VIRTUAL_CODE, keyinfo.keycode, keyinfo.modifiers, keyinfo.charcode, NULL);

    sendToClients(destList, &message);
    return 0;
}

int Register(List<uint32_t>* destList, MessageInfo* info)
{
    uint32_t id = info->arg1;
    destList->add(id);
    return 0;
}

int unregister(List<uint32_t>* destList, MessageInfo* info)
{
    uint32_t id = info->arg1;
    destList->remove(id);
    return 0;
}
