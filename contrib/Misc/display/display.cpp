#include <baygui.h>
#include <string>
#include <monapi/StringHelper.h>

using namespace std;
using namespace MonAPI;

static void __fastcall updateFeedAsync(void* arg);

class Display : public Frame {
private:
    scoped_ptr<TextField> inputArea_;
    scoped_ptr<Button> pushButton_;
    scoped_ptr<Button> updateButton_;
    std::vector<TextField*> fields_;
    typedef std::vector<Image*> Images;
    Images images_;
    bool updating_;
    int idleTimeMsec_;

public:
    enum
    {
        WIDTH = 700,
        HEIGHT = 400,
        IMAGE_WIDTH = 50,
        IMAGE_HEIGHT = 50,
        MARGIN = 15,
        MAX_ROWS = 6,
        TIMER_INTERVAL = 1000,
        forbidden_comma
    };

    Display() : inputArea_(new TextField()),
                pushButton_(new Button("Post")),
                updateButton_(new Button("Update")),
                updating_(false),
                idleTimeMsec_(5000)
    {
        setTitle("Facebook");
        setBounds(40, 40, WIDTH, HEIGHT);
        const int x = 5;
        const int y = 5;
        inputArea_->setBounds(x, y, x + 300, y + 15);
        pushButton_->setBounds(255, 30, 50, 20);
        updateButton_->setBounds(200, 30, 50, 20);
        add(inputArea_.get());
        add(pushButton_.get());
        add(updateButton_.get());
        setTimer(TIMER_INTERVAL);
    }

    ~Display()
    {
        for (Images::const_iterator it = images_.begin(); it != images_.end(); ++it) {
            delete (*it);
        }
    }

    void createOnePost(const std::string& url, const std::string& file, const std::string& text)
    {
        static int i = 0;
        WebImage* image = new WebImage(url, file);
        image->initialize();
        TextField* field = new TextField();
        fields_.push_back(field);
        images_.push_back(image);
        field->setBounds(IMAGE_WIDTH, 50 + IMAGE_HEIGHT * i, WIDTH - IMAGE_WIDTH - MARGIN, IMAGE_HEIGHT);
        add(field);
        field->setText(text.c_str());
        i++;
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
        } else if (event->getType() == Event::TIMER) {
            if (!updating_) {
                idleTimeMsec_ += TIMER_INTERVAL;
                if (idleTimeMsec_ > 5000) {
                    updateFeed();
                }
            }
            setTimer(TIMER_INTERVAL);
        }
    }

    void setStatusDone()
    {
        updateButton_->setEnabled(true);
        updateButton_->setLabel("update");
        updating_ = false;
        idleTimeMsec_ = 0;
        repaint();
    }

    void paint(Graphics *g) {
        for (size_t i = 0; i < images_.size(); i++) {
            g->drawImage(images_[i], 0, IMAGE_HEIGHT * i + 50);
        }
    }

private:
    void setStatusUpdating()
    {
        updating_ = true;
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
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    Display* display = (Display*)arg;
    uint32_t tid;
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    std::string command(System::getMoshPath());
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    command += " /LIBS/MOSH/bin/fb-feed-get.sps";
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    System::getProcessStdinID();
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    intptr_t result = monapi_call_process_execute_file_get_tid(command.c_str(), MONAPI_TRUE, &tid, System::getProcessStdinID(), System::getProcessStdoutID());
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    if (result != 0) {
        monapi_fatal("can't exec Mosh");
    }
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    monapi_process_wait_terminated(tid);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    scoped_ptr<SharedMemory> shm(monapi_file_read_all("/USER/TEMP/fb.data"));
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    if (shm.get() == NULL) {
        monapi_fatal("can't read fb.data");
    }
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    std::string text((char*)shm->data());
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    Strings lines = StringHelper::split("\n", text);
    for (size_t i = 0; i < lines.size() && i < Display::MAX_ROWS; i++) {
        Strings line = StringHelper::split("$", lines[i]);
        std::string imageUri = "http://graph.facebook.com/";
        std::string filename = "/USER/TEMP/" + line[0] + ".JPG";
        imageUri += line[0];
        imageUri += "/picture";
        display->createOnePost(imageUri, filename, line[2]);
    }
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    display->setStatusDone();
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    MessageInfo msg;
    Message::receive(&msg);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    exit(0);
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
