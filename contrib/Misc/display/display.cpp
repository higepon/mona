#include <baygui.h>
#include <string>
#include "TextArea.h"

using namespace std;
using namespace MonAPI;

class MonasqScrollbar : public Scrollbar {
    TextArea* mLinkedTextArea;

public:
    MonasqScrollbar(int orientation) :
        Scrollbar(orientation), mLinkedTextArea(NULL) {}

    void linkTextArea(TextArea* linkedTextArea) {
        mLinkedTextArea = linkedTextArea;
    }

    //値を設定する
    virtual void setValue(int value);
};


/**
 *  入力欄テキストエリア
 */
class InputArea : public TextArea {

    bool mbModified;

protected:
    //イベント処理
    void processEvent(Event *event);

public:
    InputArea(int buffer_size, bool draw_line)
        : TextArea(buffer_size, draw_line), mbModified(false) {}
    virtual ~InputArea() {}

    // 変更フラグ
    bool isModified() { return mbModified; }
    void setModifyFlag() { mbModified = true; }
    void resetModifyFlag() { mbModified = false; }
};


void InputArea::processEvent(Event *event) {
    if (event->getType() == Event::KEY_PRESSED) {
        KeyEvent* keyEvent = (KeyEvent*)event;
        int keycode = keyEvent->getKeycode();
        if (keycode < 128 || keycode == KeyEvent::VKEY_DELETE) {
            // 変更フラグを立てる
            setModifyFlag();
        }
    }
    // スーパークラスに処理を渡す
    TextArea::processEvent(event);
}

void MonasqScrollbar::setValue(int value) {
    if (value == getValue()) return;

    // スーパークラスに処理を渡す
    Scrollbar::setValue(value);

    if (mLinkedTextArea) {
        mLinkedTextArea->setVScroll(value);
    }
}


class Display : public Frame {
private:
    scoped_ptr<InputArea> textArea_;
    scoped_ptr<MonasqScrollbar> scrollbar_;

public:
    Display() : textArea_(new InputArea(500, true)),
                scrollbar_(new MonasqScrollbar(Scrollbar::VERTICAL))
    {
        if (M_OK != monapi_name_add("/applications/display")) {
            monapi_warn("name add failure");
        }
        setBounds(40, 40, 200, 200);
        setTitle("Display");
        textArea_->setBounds(5, 5, 150, 150);
        scrollbar_->setBounds(150, 5, 155, 155);
        add(textArea_.get());
        add(scrollbar_.get());
        textArea_->linkScrollbar(scrollbar_.get());
        scrollbar_->linkTextArea(textArea_.get());
    }

    ~Display()
    {
    }

    void processEvent(Event* event)
    {
         textArea_->setText(
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\nbbbbbbbbbbbbbbbbb\nccccccccccccccccc\nddddddddddddddddd\neeeeeeeeeeeeeeeee\nbbbbbbbbbbbbbbbbb\nccccccccccccccccc\nddddddddddddddddd\neeeeeeeeeeeeeeeee\nbbbbbbbbbbbbbbbbb\nccccccccccccccccc\nddddddddddddddddd\neeeeeeeeeeeeeeeee\nbbbbbbbbbbbbbbbbb\nccccccccccccccccc\nddddddddddddddddd\neeeeeeeeeeeeeeeee\nbbbbbbbbbbbbbbbbb\nccccccccccccccccc\nddddddddddddddddd\neeeeeeeeeeeeeeeee");

        if (event->getType() == Event::CUSTOM_EVENT) {
            printf("event->header=%d arg1=%d MSG_TEXT=%d\n", event->header, event->arg1, MSG_TEXT);
            if (event->header == MSG_TEXT) {
                size_t length = MESSAGE_INFO_MAX_STR_LENGTH < event->arg1 ? MESSAGE_INFO_MAX_STR_LENGTH : event->arg1;
                string text(event->str, length);
                textArea_->setText(text.c_str());
            }
        }
    }
};

int main(int argc, char* argv[])
{
#define MAP_FILE_PATH "/APPS/BAYGUI/DISPLAY.MAP"
    uint32_t pid = syscall_get_pid();
    intptr_t ret = syscall_stack_trace_enable(pid, MAP_FILE_PATH);
    if (ret != M_OK) {
        _printf("syscall_stack_trace_enable failed%d\n", ret);
    }
    Display display;
    display.run();
    return 0;
}
