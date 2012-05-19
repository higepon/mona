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

class KeyboardServer
{
private:
    HList<uint32_t> clients_;
public:
    void messageLoop(KeyBoardManager& manager)
    {
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

                    sendKeyInformation(manager, inp8(0x60));
                    break;

                case MSG_KEY_PRESS:
                {
                    MessageInfo message;
                    int charcode = info.arg1;
                    int keycode = charcode;
                    int modifiers = info.arg2 | KEY_MODIFIER_DOWN;
                    if (charcode >= 'A' && charcode <= 'Z') {
                        modifiers |= KEY_MODIFIER_SHIFT;
                    }

                    // workaround
                    if ('a' <= charcode && charcode <= 'z') {
                        // see MonaAi::Keys
                        keycode = charcode + 'a' - 'A';
                    } else if (' ' <= charcode && charcode <= '@') {
                        keycode = 0;
                    }
                    Message::create(&message, MSG_KEY_VIRTUAL_CODE, keycode, modifiers, charcode, NULL);
                    sendToClients(&message);
                    Message::reply(&info);
                    break;
                }
                case MSG_KEY_RELEASE:
                {
                    MessageInfo message;
                    Message::create(&message, MSG_KEY_VIRTUAL_CODE, info.arg1, info.arg2 | KEY_MODIFIER_UP, info.arg1, NULL);
                    sendToClients(&message);
                    Message::reply(&info);
                    break;
                }
                case MSG_ADD:

                    registerClient(info.arg1);
                    Message::reply(&info);
                    break;

                case MSG_REMOVE:

                    unRegisterClient(info.arg1);
                    Message::reply(&info);
                    break;

                default:
                    /* igonore this message */

                    break;
                }
            }
        }
    }

private:
    void sendToClients(MessageInfo* msg)
    {
        for (int i = clients_.size() - 1; i >= 0; i--)
        {
            if (Message::send(clients_[i], msg) != M_OK)
            {
                monapi_warn("send error to pid = %x", clients_[i]);
                uint32_t temp;
                clients_.removeAt(i, &temp);
            }
        }
    }

    void sendKeyInformation(KeyBoardManager& manager, uint8_t scancode)
    {
        MessageInfo message;
        KeyInfo keyinfo;

        /* scan code to virtual key information */
        if(manager.setKeyScanCode(scancode) == 0) {
            return;
        }
        manager.getKeyInfo(&keyinfo);

        memset(&message, 0, sizeof(MessageInfo));
        Message::create(&message, MSG_KEY_VIRTUAL_CODE, keyinfo.keycode, keyinfo.modifiers, keyinfo.charcode, NULL);

        sendToClients(&message);
    }

    void registerClient(uint32_t id)
    {
        clients_.add(id);
    }

    void unRegisterClient(uint32_t id)
    {
        clients_.remove(id);
    }
};

int main(int argc, char* argv[])
{
    syscall_get_io();
    intptr_t ret = monapi_enable_stacktrace("/SERVERS/KEYBDMNG.map");
    if (ret != M_OK) {
        monapi_warn("syscall_stack_trace_enable error %d\n", ret);
        exit(-1);
    }
    /* initilize KeyBoardManager */
    KeyBoardManager manager;
    manager.init();

    if (monapi_notify_server_start("MONITOR.BIN") != M_OK) {
        exit(-1);
    }
    syscall_set_irq_receiver(1, 0);
    if (monapi_name_add("/servers/keyboard") != M_OK) {
        monapi_fatal("monapi_name_add failed");
    }
    KeyboardServer server;
    server.messageLoop(manager);
    return 0;
}

