/* ps2kbc */
#include <Module.h>
#include <driver/ISADriver.h>
/*
#include <driver/interface/HID/Mouse.h>
#include <driver/interface/HID/IBMKeyboard.h>
*/
class PS2KBC {
  public:
//    Mouse *mouse;
//    IBMKeyboard *keyboard;
    PS2KBC(ISADriver *isa,Globals *g);
    ~PS2KBC();
    static byte OutBuf[32];
    static byte InBuf[32];
    static byte OutBufEndPtr;
    static byte InBufEndPtr;
    static ISADriver *isa_;
    bool Send(void);
    bool Recv(void);
};
