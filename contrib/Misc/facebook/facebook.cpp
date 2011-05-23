#include <monagui.h>
#include <string>
#include <monapi/StringHelper.h>
#include "Updater.h"
#include "FacebookService.h"
#include "FacebookPost.h"
#include "FacebookPostView.h"

using namespace std;
using namespace MonAPI;

class Facebook : public Frame {
private:
    uintptr_t updaterId_;
    scoped_ptr<TextField> inputArea_;
    scoped_ptr<Button> postButton_;
    scoped_ptr<Button> downButton_;
    scoped_ptr<Button> updateButton_;
    typedef std::vector<std::string> strings;
    typedef std::vector<FacebookPost> FacebookPosts;
    typedef std::vector<FacebookPostView*> FacebookPostViews;
    FacebookPosts posts_;
    bool updating_;
    int idleTimeMsec_;
    int offset_;
    FacebookPostViews views_;
    bool isAutoUpdate_;

public:
    enum
    {
        BUTTON_WIDTH = 50,
        BUTTON_HEIGHT = 20,
        BUTTON_MARGIN = 5,
        INPUT_AREA_WIDTH = 300,
        INPUT_AREA_HEIGHT = BUTTON_HEIGHT,
        WIDTH = 700,
        HEIGHT = 420,
        POST_HEIGHT = 50,
        MAX_ROWS = 7,
        TIMER_INTERVAL_MSEC = 1000,
        UPDATE_INTERVAL_MSEC = 30 * 1000,
        forbidden_comma
    };

    Facebook(uintptr_t updaterId) :
        updaterId_(updaterId),
        inputArea_(new TextField()),
        postButton_(new Button("Post")),
        downButton_(new Button("Down")),
        updateButton_(new Button("Update")),
        updating_(false),
        idleTimeMsec_(2000),
        offset_(0),
        isAutoUpdate_(true)
    {
        setTitle("Facebook");
        setBounds(40, 40, WIDTH, HEIGHT);
        int x = 5;
        int y = 5;
        inputArea_->setBounds(x, y, INPUT_AREA_WIDTH, INPUT_AREA_HEIGHT);
        x += INPUT_AREA_WIDTH + BUTTON_MARGIN;
        postButton_->setBounds(x, y, BUTTON_WIDTH, BUTTON_HEIGHT);
        x += BUTTON_WIDTH + BUTTON_MARGIN;
        updateButton_->setBounds(x, y, BUTTON_WIDTH, BUTTON_HEIGHT);
        x += BUTTON_WIDTH + BUTTON_MARGIN;
        downButton_->setBounds(x, y, BUTTON_WIDTH, BUTTON_HEIGHT);
        add(inputArea_.get());
        add(postButton_.get());
        add(downButton_.get());
        add(updateButton_.get());
        setTimer(TIMER_INTERVAL_MSEC);

        y += BUTTON_HEIGHT + BUTTON_MARGIN;
        for (size_t i = 0; i < MAX_ROWS; i++) {
            FacebookPostView* view = new FacebookPostView(5, y + POST_HEIGHT * i, WIDTH, POST_HEIGHT);
            views_.push_back(view);
            Components c;
            view->components(c);
            for (Components::const_iterator it = c.begin(); it != c.end(); ++it) {
                add(*it);
            }
        }
    }

    ~Facebook()
    {
    }

private:
    void postFeed()
    {
        postButton_->setEnabled(false);
        if (FacebookService::postFeed(inputArea_->getText())) {
           inputArea_->setText("");
        }
        postButton_->setEnabled(true);
        updateFeedAsync();
    }

    void updateFeedAsync()
    {
        setStatusUpdating();
        int ret = Message::send(updaterId_, MSG_UPDATE);
        if (ret != M_OK) {
            monapi_fatal("MSG_UPDATE send failed");
        }
    }

    bool readFacebookPostFromFile()
    {
        scoped_ptr<SharedMemory> shm(monapi_file_read_all("/USER/TEMP/fb.data"));
        if (shm.get() == NULL) {
            return false;
        }
        std::string text((char*)shm->data());
        Strings lines = StringHelper::split("\n", text);
        posts_.clear();
        for (size_t i = 0; i < lines.size(); i++) {
            Strings line = StringHelper::split("$", lines[i]);
            if (line.size() != 5) {
//                logprintf("lines[i]=%s size=<%d>", lines[i].c_str(), line.size());
            }
            if (line.size() == 5) {
                posts_.push_back(FacebookPost(line[0], line[1], line[2], atoi(line[3].c_str()), line[4]));
            }
        }
        return true;
    }

    void show()
    {
        if (!readFacebookPostFromFile()) {
            monapi_fatal("can't read fb.data");
        }
        setupFacebookPostViews();
    }

    void setupFacebookPostViews(size_t offset = 0)
    {
        for (size_t i = 0; i < MAX_ROWS; i++) {
            //uint64_t s1 = MonAPI::Date::nowInMsec();
            if (i + offset < posts_.size()) {
                views_[i]->setupFromFacebookPost(posts_[i + offset]);
            } else {
                views_[i]->setEmpty();
            }
            //uint64_t s2 = MonAPI::Date::nowInMsec();
//            logprintf("showFeedFromFile: createOnePost %d msec\n", (int)(s2 - s1));
        }
        setStatusDone();
    }

    bool handleLikeButtonEvent(Event* event)
    {
        if (event->getType() != MouseEvent::MOUSE_RELEASED) {
            return false;
        }
        for (FacebookPostViews::const_iterator it = views_.begin(); it != views_.end(); ++it) {
            if ((*it)->likeButton() == event->getSource()) {
                (*it)->addLike();
                updateFeedAsync();
                return true;
            }
        }
        return false;
    }

    void processEvent(Event* event)
    {
        if (event->getSource() == postButton_.get()) {
            if (event->getType() == MouseEvent::MOUSE_RELEASED) {
                postFeed();
            }
        } else if (handleLikeButtonEvent(event)) {

        } else if (event->getSource() == downButton_.get()) {
            if (event->getType() == MouseEvent::MOUSE_RELEASED) {
                isAutoUpdate_ = false;
                setupFacebookPostViews(++offset_);
                repaint();
            }
        } else if (event->getSource() == updateButton_.get()) {
            if (event->getType() == MouseEvent::MOUSE_RELEASED) {
                isAutoUpdate_ = true;
                updateFeedAsync();
            }
        } else if (event->getType() == Event::CUSTOM_EVENT) {
            if (event->header == MSG_OK && event->from == updaterId_) {
                if (event->arg1 == M_OK) {
//                    logprintf("timer update feed done MSG_OK from=%d \n", event->from);
                    show();
                } else {
                    // error, just ignore and retry next.
                    setStatusDone();
                }
            }
        } else if (event->getType() == Event::TIMER) {
            static bool isFirstTime = true;
            if (!updating_) {
                idleTimeMsec_ += TIMER_INTERVAL_MSEC;
                if (isFirstTime || (isAutoUpdate_ && idleTimeMsec_ > UPDATE_INTERVAL_MSEC)) {
                    isFirstTime = false;
//                    logprintf("timer update feed start\n");
                    updateFeedAsync();
                }
            }
            setTimer(TIMER_INTERVAL_MSEC);
        } else {
        }
    }

    void setStatusDone()
    {
        updateButton_->setEnabled(true);
        updateButton_->setLabel("update");
        updating_ = false;
        idleTimeMsec_ = 0;
//        uint64_t s = MonAPI::Date::nowInMsec();
        repaint();
        //uint64_t e = MonAPI::Date::nowInMsec();
//        logprintf("repaint %d msec\n", (int)(e - s));
    }

    void paint(Graphics *g)
    {
        for (FacebookPostViews::const_iterator it = views_.begin(); it != views_.end(); ++it) {
            (*it)->drawImage(g);
        }
    }

    void setStatusUpdating()
    {
        updating_ = true;
        updateButton_->setEnabled(false);
        updateButton_->setLabel("updating");
    }
};

static void __fastcall updaterLauncher(void* arg)
{
    Updater updater;
    updater.run();
}

int main(int argc, char* argv[])
{
#define MAP_FILE_PATH "/APPS/MONAGUI/FACEBOOK.MAP"
    uint32_t pid = syscall_get_pid();
    intptr_t ret = syscall_stack_trace_enable(pid, MAP_FILE_PATH);
    if (ret != M_OK) {
        _printf("syscall_stack_trace_enable failed%d\n", ret);
    }

    uintptr_t updaterId = monapi_thread_create_with_arg(updaterLauncher, NULL);
    Facebook facebook(updaterId);
    facebook.run();
    return 0;
}
