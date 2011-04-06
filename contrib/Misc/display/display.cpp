#include <baygui.h>
#include <string>
#include <monapi/StringHelper.h>

using namespace std;
using namespace MonAPI;

static void __fastcall updateFeedAsync(void* arg);

class Display : public Frame {
private:
    scoped_ptr<TextField> inputArea_;
    scoped_ptr<TextField> outputArea_;
    scoped_ptr<Button> pushButton_;
    scoped_ptr<Button> updateButton_;
    std::vector<TextField*> fields_;
    WebImage* image_;

public:
    Display() : inputArea_(new TextField()),
                outputArea_(new TextField()),
                pushButton_(new Button("Post")),
                updateButton_(new Button("Update")),
                image_(NULL)
    {
//        image_->initialize();
        setTitle("Facebook");
        setBounds(40, 40, 700, 400);
        const int width = 300;
        const int height = 15;
        const int x = 5;
        const int y = 5;
        inputArea_->setBounds(x, y, x + width, y + height);
        outputArea_->setBounds(x, y + 100, 600, 370);
        pushButton_->setBounds(255, 30, 50, 20);
        updateButton_->setBounds(200, 30, 50, 20);
        add(inputArea_.get());
        add(outputArea_.get());
        add(pushButton_.get());
        add(updateButton_.get());
    }

    ~Display()
    {
    }

    // temporary
    void createWebImage(const std::string& url, const std::string& file, const std::string& text)
    {
        image_ = new WebImage(url, file);
        image_->initialize();
        TextField* field = new TextField();
        fields_.push_back(field);
        field->setBounds(50, 50, 500, 50);
        add(field);
        field->setText(text.c_str());
        repaint();
    }

    void postFeed()
    {
        uint32_t tid;
        std::string ret(System::getMoshPath());
        ret += " /LIBS/MOSH/bin/fb-feed-post.sps ";
        ret += inputArea_->getText();
        int result = monapi_call_process_execute_file_get_tid(ret.c_str(), MONAPI_TRUE, &tid, System::getProcessStdinID(), System::getProcessStdoutID());
        if (result != 0) {
            monapi_fatal("can't exec Mosh");
        }
    }

    void updateFeed()
    {
        setStatusUpdating();
        syscall_mthread_create_with_arg(updateFeedAsync, this);
    }

    void setFeedText(const std::string& text)
    {
        outputArea_->setText(text.c_str());
    }

    void processEvent(Event* event)
    {
        if (event->getSource() == pushButton_.get()) {
            if (event->getType() == MouseEvent::MOUSE_RELEASED) {
                postFeed();
            }
        } else if (event->getSource() == updateButton_.get()) {
            if (event->getType() == MouseEvent::MOUSE_RELEASED) {
                updateFeed();
            }
        }
    }

    void setStatusDone()
    {
        updateButton_->setEnabled(true);
        updateButton_->setLabel("update");
    }

    void paint(Graphics *g) {
        if (image_) {
            g->drawImage(image_, 0, 50);
        }
    }



private:
    void setStatusUpdating()
    {
        updateButton_->setEnabled(false);
        updateButton_->setLabel("updating");
    }

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

static void __fastcall updateFeedAsync(void* arg)
{
    Display* display = (Display*)arg;
    uint32_t tid;
    std::string command(System::getMoshPath());
    command += " /LIBS/MOSH/bin/fb-feed-get.sps";
    intptr_t result = monapi_call_process_execute_file_get_tid(command.c_str(), MONAPI_TRUE, &tid, System::getProcessStdinID(), System::getProcessStdoutID());
    if (result != 0) {
        monapi_fatal("can't exec Mosh");
    }
    monapi_process_wait_terminated(tid);
    scoped_ptr<SharedMemory> shm(monapi_file_read_all("/USER/TEMP/fb.data"));
    if (shm.get() == NULL) {
        monapi_fatal("can't read fb.data");
    }
    std::string text((char*)shm->data());
    Strings lines = StringHelper::split("\n", text);
    Strings line = StringHelper::split("$", lines[0]);
    std::string imageUri = "http://graph.facebook.com/";
    std::string filename = "/USER/TEMP/" + line[0] + ".JPG";
    imageUri += line[0];
    imageUri += "/picture?type=small";
    display->createWebImage(imageUri, filename, line[2]);
    display->setFeedText((char*)shm->data());
    display->setStatusDone();
}


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
