#include<Sub.h>

int Sub::getNumber(void) {
    return 777;
}

char* Sub::getName(void) {

    static char* name = "BASE\n";
    return name;
}
