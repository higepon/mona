/* mouse.h (expr)*/
#ifndef __EXPR__MOUSE_H
#define __EXPR__MOUSE_H
#include <types.h>
#include <driver/ISADriver.h>
class Mouse {
  public:
    Mouse(ISADriver *isa);
    ~Mouse();
    static dword x;
    static dword y;
    static dword z;
    static dword btncount[3];
    static bool btnstate[3];
  private:
    static ISADriver *isa_;
    void Send(byte b);
    byte Recv();
};
#endif
