#include <userlib.h>

void main() {

    for (;;) {
        sleep(100);
        print("Mona!!!_( ;'[]`)_");
        sleep(100);
        print("Mona!!!|( ;'<>`)/");

        for (int x = 10;  x < 150; x++) {
            for (char color = 0; color < 16; color++) {
                _put_pixel(x, 5 + color, color);
            }
        }
    }
}
