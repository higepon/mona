#include <userlib.h>
#include <KeyBoardManager.h>

int main() {

    KeyBoardManager* manager = new KeyBoardManager();
    manager->init();

    Message message;
    KeyInfo info;
    for (;;) {
        if (!_receive(&message)) {

            printf("[1]");
            byte scancode = message.arg1;
            printf("[2]");
            manager->setKeyScanCode(scancode);
            printf("[3]");
            manager->getKeyInfo(&info);
            printf("[4]");
            memset(&message, 0, sizeof(Message));
            printf("[5]");
            message.arg1 = info.keycode;
            message.arg2 = info.modifiers;

            printf("[6]");
            if (_send("USER.ELF", &message)) {
                print("send error to user elf");
            }

        }
    }
}
