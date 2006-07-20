#include <monalibc.h>

void memswap(char *a, char *b, size_t size) {

    size_t i;
    char tmp;

    for(i = 0; i < size; i++) {
        tmp = a[i];
        a[i] = b[i];
        b[i] = tmp;
    }

    return;
}

