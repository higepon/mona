#include "mysetjmp.h"

asm(".section .drectve");
asm(".ascii \"-export:_mysetjmp\n\"");
asm(".ascii \"-export:_mylongjmp\n\"");
