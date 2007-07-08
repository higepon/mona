#include <monapi.h>
#include <monapi/messages.h>

namespace MonAPI {

/*----------------------------------------------------------------------
    Global
----------------------------------------------------------------------*/
MonaApplication* monaApp;

/*----------------------------------------------------------------------
    MonaApplication
----------------------------------------------------------------------*/
MonaApplication::MonaApplication() {

    monaApp = this;
    if (!monapi_register_to_server(ID_KEYBOARD_SERVER, MONAPI_TRUE))
    {
        MONAPI_WARN("MonaApplication:KeyBoardServer not found");
        exit(1);
    }
}

MonaApplication::~MonaApplication() {

    if (!monapi_register_to_server(ID_KEYBOARD_SERVER, MONAPI_FALSE)) {
        MONAPI_WARN("MonaApp: key unregist error");
    }
}

/*----------------------------------------------------------------------
    messageLoop for MonaApplication
----------------------------------------------------------------------*/
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
