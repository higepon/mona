/* Keyboard class */
#include <driver/interface/MessageHandler.h>

typedef byte keycode;
class IBMKeyBoardMessageHandler : public MessageHandler {
  public:
    virtual void OnKeyDown(keycode k);
    virtual void OnKeyUp(keycode k);
};
