#include <monapi.h>
#include <monapi/messages.h>

namespace MonAPI {

/*----------------------------------------------------------------------
    Global
----------------------------------------------------------------------*/
MonaApplication* monaApp;

/*----------------------------------------------------------------------
    messageLoop for MonaApplication
----------------------------------------------------------------------*/
void messageLoop() {

    volatile MessageInfo message;

    for (;;) {
        if (!Message::receive((MessageInfo*)(&message))) {

            switch(message.header) {

            case MSG_KEY_VIRTUAL_CODE:

                monaApp->onKeyDown(message.arg1, message.arg2);
                break;

            case MSG_MOUSE_INFO:

                if (message.arg3 & 0x01) {
                    monaApp->onMouseClick((int)(message.arg1), (int)(message.arg2));
                }
                break;
            default:

                /* ignore this message */
                break;
            }
        }
    }
}

/*----------------------------------------------------------------------
    MonaApplication
----------------------------------------------------------------------*/
MonaApplication::MonaApplication() {

    monaApp = this;

    int id = syscall_mthread_create((dword)MESSAGE_LOOP);
    syscall_mthread_join(id);

    mypid_ = System::getThreadID();

    dword targetID = Message::lookupMainThread("KEYBDMNG.BIN");
    if (targetID == 0xFFFFFFFF)
    {
        printf("MonaApplication:KeyBoardServer not found\n");
        exit(1);
    }

    /* create message for KEYBDMNG.BIN */
    MessageInfo info;
    info.header = MSG_KEY_REGIST_TO_SERVER;
    info.arg1   = mypid_;

    /* send */
    if (Message::send(targetID, &info)) {
        printf("MonaApp: key regist error\n");
    }
}

MonaApplication::~MonaApplication() {

    /* create message for KEYBDMNG.BIM */
    MessageInfo info;
    info.header = MSG_KEY_UNREGIST_FROM_SERVER;
    info.arg1   = mypid_;

    dword targetID = Message::lookupMainThread("KEYBDMNG.BIN");
    if (targetID == 0xFFFFFFFF)
    {
        printf("MonaApplication:KeyBoardServer not found\n");
        exit(1);
    }

    /* send */
    if (Message::send(targetID, &info)) {
        printf("MonaApp: key unregist error\n");
    }
}

void MonaApplication::run() {

    volatile MessageInfo message;

    for (;;) {
        if (!Message::receive((MessageInfo*)(&message))) {

            switch(message.header) {

            case MSG_KEY_VIRTUAL_CODE:

                this->onKeyDown(message.arg1, message.arg2);
                break;

            case MSG_MOUSE_INFO:

                if (message.arg3 & 0x01) {
                    this->onMouseClick((int)(message.arg1), (int)(message.arg2));
                }
                break;
            default:

                /* ignore this message */
                break;
            }
        }
    }
}

}
