#include <baygui.h>
#include <string>

using namespace std;
using namespace MonAPI;
//using namespace baygui;
// class MonasqScrollbar : public Scrollbar {
//     TextArea* mLinkedTextArea;

// public:
//     MonasqScrollbar(int orientation) :
//         Scrollbar(orientation), mLinkedTextArea(NULL) {}

//     void linkTextArea(TextArea* linkedTextArea) {
//         mLinkedTextArea = linkedTextArea;
//     }

//     virtual void setValue(int value)
//     {
//         if (value == getValue()) return;

//         Scrollbar::setValue(value);

//         if (mLinkedTextArea) {
//             mLinkedTextArea->setVScroll(value);
//         }
//     }
// };


// class InputArea : public TextArea {

//     bool mbModified;

// protected:
//     void processEvent(Event *event);

// public:
//     InputArea(int buffer_size, bool draw_line)
//         : TextArea(buffer_size, draw_line), mbModified(false) {}
//     virtual ~InputArea() {}

//     bool isModified() { return mbModified; }
//     void setModifyFlag() { mbModified = true; }
//     void resetModifyFlag() { mbModified = false; }
// };

// void InputArea::processEvent(Event *event) {
//     if (event->getType() == Event::KEY_PRESSED) {
//         KeyEvent* keyEvent = (KeyEvent*)event;
//         int keycode = keyEvent->getKeycode();
//         if (keycode < 128 || keycode == KeyEvent::VKEY_DELETE) {
//             setModifyFlag();
//         }
//     }
//     TextArea::processEvent(event);
// }


class Display : public Frame {
private:
    scoped_ptr<TextField> textArea_;
    scoped_ptr<Button> button_;

public:
    Display() : textArea_(new TextField()),
                button_(new Button("hige"))
    {
        if (M_OK != monapi_name_add("/applications/display")) {
            monapi_warn("name add failure");
        }
        setTitle("*Display*");
        setBounds(40, 40, 400, 200);
        const int width = 300;
        const int height = 145;
        const int x = 5;
        const int y = 5;
        textArea_->setBounds(x, y, x + width, y + height);
#if 0
        add(textArea_.get());
        add(scrollbar_.get());
#endif
        add(textArea_.get());
        add(button_.get());
    }

    ~Display()
    {
    }

    void executeMosh()
    {
        uint32_t tid;
        std::string ret = "/APPS/MOSH.APP/MOSH.EXE --loadpath=/LIBS/MOSH/lib /USER/POST.SCM ";
        ret += textArea_->getText();
        int result = monapi_call_process_execute_file_get_tid(ret.c_str(), MONAPI_TRUE, &tid, System::getProcessStdinID(), System::getProcessStdoutID());
        if (result != 0) {
            monapi_fatal("can't exec Mosh");
        }
    }

    void processEvent(Event* event)
    {
        if (event->getSource() == button_.get()) {
            if (event->getType() == MouseEvent::MOUSE_PRESSED) {
                executeMosh();
            }

        }
        // if (event->getType() == Event::CUSTOM_EVENT) {
        //     if (event->header == MSG_TEXT) {
        //         size_t length = MESSAGE_INFO_MAX_STR_LENGTH < event->arg1 ? MESSAGE_INFO_MAX_STR_LENGTH : event->arg1;
        //         string text(event->str, length);
        //         string content(textArea_->getText());
        //         content += foldLine(text);
        //         textArea_->setText(content.c_str());
        //         repaint();
        //     }
        // }
    }

private:
    string foldLine(const string& line)
    {
        const size_t MAX_LINE_LEN = 45;
        size_t len = 0;
        string ret;
        for (string::const_iterator it = line.begin(); it != line.end(); ++it) {
            ret += *it;
            if ((*it) == '\n') {
                len = 0;
            } else {
                len++;
            }

            if (len >= MAX_LINE_LEN) {
                ret += '\n';
                len = 0;
            }
        }
        return ret;
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
