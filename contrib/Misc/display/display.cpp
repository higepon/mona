#include <baygui.h>
#include <string>
#include <monapi/StringHelper.h>
#include "Updater.h"
#include "FacebookService.h"
#include "FacebookPost.h"
#include "FacebookPostView.h"

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
        postButton_(new Button("Post")),
        downButton_(new Button("Down")),
        updateButton_(new Button("Update")),
        updating_(false),
        idleTimeMsec_(2000),
        offset_(0)
    {
        setTitle("Facebook");
        setBounds(40, 40, WIDTH, HEIGHT);
        const int x = 5;
        const int y = 5;
        inputArea_->setBounds(x, y, x + 300, y + 15);
        postButton_->setBounds(255, 30, 50, 20);
        updateButton_->setBounds(200, 30, 50, 20);
        add(inputArea_.get());
        add(postButton_.get());
        add(downButton_.get());
        add(updateButton_.get());
        setTimer(TIMER_INTERVAL);

        for (size_t i = 0; i < MAX_ROWS; i++) {
            FacebookPostView* view = new FacebookPostView(IMAGE_WIDTH, 50 + IMAGE_HEIGHT * i, WIDTH - IMAGE_WIDTH - MARGIN, IMAGE_HEIGHT);
            views_.push_back(view);
            Components c;
            view->components(c);
            for (Components::const_iterator it = c.begin(); it != c.end(); ++it) {
                add(*it);
            }
        }
    }

    ~Display()
    {
    }

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

    void createOnePost(const std::string& url, const std::string& file, const std::string& text, int index)
    {
        FacebookPostView* view = views_[index];
        view->setImagePath(url, file);
        String x = text.c_str();
        view->setText(foldLine(text, 70));
    }

    void postLike(const std::string& postId)
    {
        if (FacebookService::addLike(postId)) {
            inputArea_->setText("");
        }
        updateFeed();
    }

    void postFeed()
    {
        postButton_->setEnabled(false);
        if (FacebookService::postFeed(inputArea_->getText())) {
           inputArea_->setText("");
        }
        postButton_->setEnabled(true);
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

    bool readFeedFromFile()
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
                logprintf("lines[i]=%s size=<%d>", lines[i].c_str(), line.size());
            }
            if (line.size() == 5) {
                posts_.push_back(FacebookPost(line[0], line[1], line[2], atoi(line[3].c_str()), line[4]));
            }
        }
        return true;
    }

    void show()
    {
        if (!readFeedFromFile()) {
            monapi_fatal("can't read fb.data");
        }
        showFeedFromFile();
    }

    void showFeedFromFile(size_t offset = 0)
    {

        for (size_t i = offset; i < posts_.size() && i < MAX_ROWS; i++) {
            uint64_t s1 = MonAPI::Date::nowInMsec();
            std::string content = posts_[i].text;
            // if (posts_[i].numLikes > 0) {
            //     content += "\n";
            //     char buf[32];
            //     sprintf(buf, "%d", posts_[i].numLikes);
            //     content += buf;
            //     content += "人がいいね！と言っています。";
            // }
            createOnePost(posts_[i].imageUrl(), posts_[i].localImagePath(), content, i);
            uint64_t s2 = MonAPI::Date::nowInMsec();
            logprintf("showFeedFromFile: createOnePost %d msec\n", (int)(s2 - s1));
        }
        setStatusDone();
    }

    int sourceIsLikeButton(Event* event)
    {
        // for (size_t i = 0; i < likeButtons_.size(); i++) {
        //     if (event->getSource() == likeButtons_[i]) {
        //         return i;
        //     }
        // }
        return -1;
    }

    void processEvent(Event* event)
    {
        if (event->getSource() == postButton_.get()) {
            if (event->getType() == MouseEvent::MOUSE_RELEASED) {
                postFeed();
            }
        } else if (event->getSource() == downButton_.get()) {
            if (event->getType() == MouseEvent::MOUSE_RELEASED) {
                showFeedFromFile(++offset_);
            }
        } else if (event->getSource() == updateButton_.get()) {
            if (event->getType() == MouseEvent::MOUSE_RELEASED) {
                updateFeed();
            }
        } else if (event->getType() == Event::CUSTOM_EVENT) {
            if (event->header == MSG_OK && event->from == updaterId_) {
                if (event->arg1 == M_OK) {
                    logprintf("timer update feed done MSG_OK from=%d \n", event->from);
                    show();
                } else {
                    // error, just ignore and retry next.
                    setStatusDone();
                }
            }
        } else if (event->getType() == Event::TIMER) {
            if (!updating_) {
                idleTimeMsec_ += TIMER_INTERVAL;
                if (idleTimeMsec_ > 2000) {
                    logprintf("timer update feed start\n");
                    updateFeed();
                }
            }
            setTimer(TIMER_INTERVAL);
        } else {
            int likeButtonIndex = sourceIsLikeButton(event);
            if (likeButtonIndex != -1) {
                if (event->getType() == MouseEvent::MOUSE_RELEASED) {
                    ASSERT((size_t)likeButtonIndex < posts_.size());
                    postLike(posts_[likeButtonIndex].postId);
                }
            }
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
