#include <userlib.h>

int sub(int i) {


    i--;
    return i;
}


void main() {

    while (true) {

        sub(5);
//         syscall_sleep(100);
//         syscall_print("Mona!!!_( ;'[]`)_");
//         syscall_sleep(100);
//         syscall_print("Mona!!!|( ;'<>`)/");
    }
}

