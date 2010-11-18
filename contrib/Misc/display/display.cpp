#include <baygui.h>
#include <string>

using namespace std;
using namespace MonAPI;

class Display : public Frame {
private:
    scoped_ptr<Label> label_;

public:
    Display() : label_(new Label("", Label::CENTER))
    {
        setBounds(40, 40, 200, 200);
        setTitle("Display");
        label_->setBounds(5, 5, 150, 150);
        add(label_.get());
    }

    ~Display()
    {
    }

    void processEvent(Event* event)
    {
        if (event->getType() == Event::CUSTOM_EVENT) {
            if (event->header == MSG_TEXT) {
                size_t length = MESSAGE_INFO_MAX_STR_LENGTH < event->arg1 ? MESSAGE_INFO_MAX_STR_LENGTH : event->arg1;
                string text(event->str, length);
                label_->setText(text.c_str());
            }
        }
    }
};

int main(int argc, char* argv[])
{
    Display display;
    display.run();
    return 0;
}
