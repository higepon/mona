#include <userlib.h>

void main() {

    while (true) {
        syscall_sleep(100);
        syscall_print("Mona!!!_( ;'[]`)_");
        syscall_sleep(100);
        syscall_print("Mona!!!|( ;'<>`)/");
    }
}
