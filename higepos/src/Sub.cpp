#include<Sub.h>

int Sub::getNumber(void) {
    return 777;
}

char* Sub::getName(void) {

    static char* name = "SAVE\n";
    return name;
}

Sub::Sub() {
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("hlt");
    return;
}
