#include <userlib.h>
#include <KeyBoardManager.h>

int main() {

    KeyBoardManager* manager = new KeyBoardManager();
    manager->init();

    KMessage message;
    KeyInfo info;
    for (;;) {
        if (!_receive(&message)) {

            byte scancode = message.arg1;
            manager->setKeyScanCode(scancode);
            manager->getKeyInfo(&info);
            memset(&message, 0, sizeof(KMessage));
            message.arg1 = info.keycode;
            message.arg2 = info.modifiers;

            if (_send("USER.ELF", &message)) {
                print("send error to user elf");
            }

        }
    }
}
