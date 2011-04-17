#include <monapi.h>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>

using namespace MonAPI;

// Used by other test programs.
// Wait until MSG_STOP comes.

static void wait()
{
    for (MessageInfo msg;;) {
        if (Message::receive(&msg) != M_OK) {
            continue;
        }
        switch (msg.header) {
        case MSG_STOP:
            return;
        default:
            break;
        }
    }
}
const int ABNORMAL_STATUS = 13;
int main(int argc, char *argv[])
{
    wait();
    exit(ABNORMAL_STATUS);
}
