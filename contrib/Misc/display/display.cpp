#include <baygui.h>
#include <string>

using namespace std;
using namespace MonAPI;

class Display : public Frame {
private:
    scoped_ptr<TextField> inputArea_;
    scoped_ptr<TextField> outputArea_;
    scoped_ptr<Button> button_;

public:
    Display() : inputArea_(new TextField()),
                outputArea_(new TextField()),
                button_(new Button("Post"))
    {
        setTitle("Facebook");
        setBounds(40, 40, 400, 200);
        const int width = 300;
        const int height = 15;
        const int x = 5;
        const int y = 5;
        inputArea_->setBounds(x, y, x + width, y + height);
        outputArea_->setBounds(x, y + 100, x + width, y + 50);
        button_->setBounds(255, 30, 50, 20);
        add(inputArea_.get());
        add(outputArea_.get());
        add(button_.get());
    }

    ~Display()
    {
    }

    intptr_t wait(uintptr_t tid)
    {
        for (MessageInfo msg;;) {
            if (MonAPI::Message::receive(&msg) != 0) continue;
            switch (msg.header)
            {
            case MSG_PROCESS_TERMINATED:
                if (tid == msg.arg1)
                {
                    // exit status code
                    return msg.arg2;
                }
                break;
            default:
                break;
            }
        }
    }

    void executeMosh()
    {
        uint32_t tid;
        std::string ret = "/APPS/MOSH.APP/MOSH.EXE --loadpath=/LIBS/MOSH/lib /USER/POST.SCM ";
        ret += inputArea_->getText();
        int result = monapi_call_process_execute_file_get_tid(ret.c_str(), MONAPI_TRUE, &tid, System::getProcessStdinID(), System::getProcessStdoutID());
        if (result != 0) {
            monapi_fatal("can't exec Mosh");
        }
        result = monapi_call_process_execute_file_get_tid("/APPS/MOSH.APP/MOSH.EXE --loadpath=/LIBS/MOSH/lib /USER/GET.SCM", MONAPI_TRUE, &tid, System::getProcessStdinID(), System::getProcessStdoutID());
        if (result != 0) {
            monapi_fatal("can't exec Mosh");
        }
        button_->setEnabled(false);
        wait(tid);
        scoped_ptr<SharedMemory> shm(monapi_file_read_all("/USER/TEMP/fb.data"));
        outputArea_->setText((char*)shm->data());
    }

    void processEvent(Event* event)
    {
        if (event->getSource() == button_.get()) {
            if (event->getType() == MouseEvent::MOUSE_PRESSED) {
                executeMosh();
            }
        }
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
