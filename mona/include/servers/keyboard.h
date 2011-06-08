#ifndef _SERVER_KEYBOARD_
#define _SERVER_KEYBOARD_

enum {

    MSG_KEY_VIRTUAL_CODE = 0x5131510e, /* "KEY:VCD" */
    MSG_KEY_PRESS = 0x5130f8ca,        /* "KEY:PRS" */
    MSG_KEY_RELEASE = 0x513115ca       /* "KEY:RLS" */
};

#endif
