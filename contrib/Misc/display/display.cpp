#include <baygui.h>
#include <string>
#include <monapi/StringHelper.h>
#include "Updater.h"

using namespace std;
using namespace MonAPI;

class Display : public Frame {

public:
    enum
    {
        WIDTH = 700,
        HEIGHT = 400,
        IMAGE_WIDTH = 50,
        IMAGE_HEIGHT = 50,
        MARGIN = 15,
        MAX_ROWS = 6,
        TIMER_INTERVAL = 5000,
        forbidden_comma
    };

    Display(uintptr_t updaterId) :
        updaterId_(updaterId),
        inputArea_(new TextField()),
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

        for (size_t i = 0; i < MAX_ROWS; i++) {
            TextField* field = new TextField();
            fields_.push_back(field);
            field->setBounds(IMAGE_WIDTH, 50 + IMAGE_HEIGHT * i, WIDTH - IMAGE_WIDTH - MARGIN, IMAGE_HEIGHT);
            add(field);
        }
    }

    ~Display()
    {
        disposeImages();
    }

private:
    uintptr_t updaterId_;
    scoped_ptr<TextField> inputArea_;
    scoped_ptr<Button> pushButton_;
    scoped_ptr<Button> updateButton_;
    typedef std::vector<TextField*> TextFields;
    typedef std::vector<Image*> Images;
    TextFields fields_;
    Images images_;
    bool updating_;
    int idleTimeMsec_;

    void disposeImages()
    {
        for (Images::const_iterator it = images_.begin(); it != images_.end(); ++it) {
            delete (*it);
        }
        images_.clear();
    }

    void disposeTextFields()
    {
        for (TextFields::const_iterator it = fields_.begin(); it != fields_.end(); ++it) {
            remove(*it);
            delete (*it);
        }
        fields_.clear();
    }

    void createOnePost(const std::string& url, const std::string& file, const std::string& text, int index)
    {
        WebImage* image = new WebImage(url, file);
        image->initialize();

        // this is workaround, should be check on Image class, but doesn't work
        if (image->getWidth() != 0) {
            image->resize(20, 20);
        }
        images_.push_back(image);
        String x = text.c_str();
        fields_[index]->setText(foldLine(text, 70).c_str());
    }

    void postFeed()
    {
        pushButton_->setEnabled(false);
        uint32_t tid;
        std::string command(System::getMoshPath());
        command += " /LIBS/MOSH/bin/fb-feed-post.sps \"";
        command += inputArea_->getText();
        command += "\"";
        int result = monapi_process_execute_file_get_tid(command.c_str(),
                                                              MONAPI_TRUE,
                                                              &tid,
                                                              System::getProcessStdinID(),
                                                              System::getProcessStdoutID());
        if (result != 0) {
            monapi_fatal("can't exec Mosh");
        }
        if (0 == monapi_process_wait_terminated(tid)) {
            inputArea_->setText("");
        }
        pushButton_->setEnabled(true);
        updateFeed();
    }

    void updateFeed()
    {
        setStatusUpdating();
        int ret = Message::send(updaterId_, MSG_UPDATE);
        if (ret != M_OK) {
            monapi_fatal("MSG_UPDATE send failed");
        }
    }

    void showFeedFromFile()
    {
        uint64_t s = MonAPI::Date::nowInMsec();
        scoped_ptr<SharedMemory> shm(monapi_file_read_all("/USER/TEMP/fb.data"));
        if (shm.get() == NULL) {
            monapi_fatal("can't read fb.data");
        }
        uint64_t s0 = MonAPI::Date::nowInMsec();
        logprintf("showFeedFromFile: readfile %d msec\n", (int)(s0 - s));
        disposeImages();
//        disposeTextFields();
        std::string text((char*)shm->data());
        Strings lines = StringHelper::split("\n", text);
        for (size_t i = 0; i < lines.size() && i < MAX_ROWS; i++) {
            Strings line = StringHelper::split("$", lines[i]);
            std::string imageUri = "http://graph.facebook.com/";
            std::string filename = "/USER/TEMP/" + line[0] + ".JPG";
            imageUri += line[0];
            imageUri += "/picture";
            uint64_t s1 = MonAPI::Date::nowInMsec();
            createOnePost(imageUri, filename, line[2], i);
            uint64_t s2 = MonAPI::Date::nowInMsec();
            logprintf("showFeedFromFile: createOnePost %d msec\n", (int)(s2 - s1));
        }
        uint64_t e = MonAPI::Date::nowInMsec();
        logprintf("showFeedFromFile %d msec\n", (int)(e - s));
        setStatusDone();
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
        } else if (event->getType() == Event::CUSTOM_EVENT) {
            if (event->header == MSG_OK && event->from == updaterId_) {
                if (event->arg1 == M_OK) {
                    logprintf("timer update feed done MSG_OK from=%d \n", event->from);
                    showFeedFromFile();
                } else {
                    // error, just ignore and retry next.
                    setStatusDone();
                }
            }
        } else if (event->getType() == Event::TIMER) {
            if (!updating_) {
                idleTimeMsec_ += TIMER_INTERVAL;
                if (idleTimeMsec_ > 5000) {
                    logprintf("timer update feed start\n");
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
        uint64_t s = MonAPI::Date::nowInMsec();
        repaint();
        uint64_t e = MonAPI::Date::nowInMsec();
        logprintf("repaint %d msec\n", (int)(e - s));
    }

    void paint(Graphics *g)
    {
        for (size_t i = 0; i < images_.size(); i++) {
            g->drawImage(images_[i], 0, IMAGE_HEIGHT * i + 50);
        }
    }

    void setStatusUpdating()
    {
        updating_ = true;
        updateButton_->setEnabled(false);
        updateButton_->setLabel("updating");
    }

    string foldLine(const string& line, size_t maxLineLength)
    {
        size_t len = 0;
        string ret;
        for (string::const_iterator it = line.begin(); it != line.end(); ++it) {
            ret += *it;
            if ((*it) == '\n') {
                len = 0;
            } else {
                len++;
            }

            if (len >= maxLineLength) {
                ret += '\n';
                len = 0;
            }
        }
        return ret;
    }
};

static void __fastcall updaterLauncher(void* arg)
{
    Updater updater;
    updater.run();
}

int main(int argc, char* argv[])
{
#define MAP_FILE_PATH "/APPS/BAYGUI/DISPLAY.MAP"
    uint32_t pid = syscall_get_pid();
    intptr_t ret = syscall_stack_trace_enable(pid, MAP_FILE_PATH);
    if (ret != M_OK) {
        _printf("syscall_stack_trace_enable failed%d\n", ret);
    }

    uintptr_t updaterId = monapi_thread_create_with_arg(updaterLauncher, NULL);
    Display display(updaterId);
    display.run();
    return 0;
}
