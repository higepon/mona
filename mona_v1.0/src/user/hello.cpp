#include <userlib.h>

int main() {

    Message message;
    char buf[32];
    for (;;) {

	//P        if (!_receive(&message)) {

//             buf[0] = '<';
//             buf[1] = (char)message.arg1;
//             buf[2] = '>';
//             buf[3] = '\0';
//             print(buf);
	print("here");
//        }
    }

    return 0;
}
