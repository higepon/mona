#include <setjmp.h>

asm(".section .drectve");
asm(".ascii \"-export:_setjmp\n\"");
asm(".ascii \"-export:_longjmp\n\"");
