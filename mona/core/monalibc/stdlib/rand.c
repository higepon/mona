#include <stdlib.h>

int rand() {

    rand_next = (rand_next ^ 0x98765432)*0x1357;

    return rand_next % RAND_MAX;
}
