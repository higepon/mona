#include <monagui.h>
#include <string>
#include <monapi/StringHelper.h>
#include "Updater.h"
#include "FacebookService.h"
#include "FacebookPost.h"
#include "FacebookPostView.h"

using namespace std;
using namespace MonAPI;

class ShareButton : public Button
{
public:
    ShareButton() : Button("Share")
    {
    }

    virtual ~ShareButton()
    {
    }

    void paint(Graphics* g)
    {
        int w = getWidth();
        int h = getHeight();

        g->setColor(0x29, 0x45, 0x7f);
        g->fillRect(0, 0, w, h);
        g->setColor(0x5f, 0x78, 0xab);
        g->fillRect(1, 1, w - 2, h - 2);
        if (getPushed()) {
            g->setColor(monagui::Color::white);
            g->drawLine(2, h - 2, w - 3, h - 2);
            g->drawLine(w - 2, 2, w - 2, h - 3);
            g->drawLine(w - 3 , h - 3, w - 3, h - 3);
            g->setColor(monagui::Color::gray);
            g->drawLine(1, 2, 1, h - 3);
            g->drawLine(2, 1, w - 3, 1);
        }
        int fw = getFontMetrics()->getWidth(getLabel());
        int fh = getFontMetrics()->getHeight(getLabel());
        int x = (w - fw) / 2;
        int y = (h - fh) / 2;
        if (getPushed()) {
            x++;
            y++;
        }
        g->setColor(monagui::Color::white);
        g->setFontStyle(Font::BOLD);
        g->drawString(getLabel(), x, y);
    }
};

class Facebook : public Frame {
private:
    uintptr_t updaterId_;
    scoped_ptr<TextField> inputArea_;
    scoped_ptr<Button> shareButton_;
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
        HEIGHT = 435,
        POST_HEIGHT = 50,
        MAX_ROWS = 7,
        TIMER_INTERVAL_MSEC = 1000,
        UPDATE_INTERVAL_MSEC = 30 * 1000,
        forbidden_comma
    };

    Facebook(uintptr_t updaterId) :
        updaterId_(updaterId),
        inputArea_(new TextField()),
        shareButton_(new ShareButton()),
        downButton_(new FBButton(">>>")),
        updateButton_(new FBButton("Update")),
        updating_(false),
        idleTimeMsec_(2000),
        offset_(0),
        isAutoUpdate_(true)
    {
        setTitle("Facebook");
        setBackground(monagui::Color::white);
        setBounds(40, 40, WIDTH, HEIGHT);
        inputArea_->setForeground(0xffcccccc);
        int x = 5;
        int y = 5;
        inputArea_->setBounds(x, y, INPUT_AREA_WIDTH, INPUT_AREA_HEIGHT);
        x += INPUT_AREA_WIDTH + BUTTON_MARGIN;
        shareButton_->setBounds(x, y, BUTTON_WIDTH, BUTTON_HEIGHT);
        x += BUTTON_WIDTH + BUTTON_MARGIN;
        updateButton_->setBounds(x, y, BUTTON_WIDTH, BUTTON_HEIGHT);
        x += BUTTON_WIDTH + BUTTON_MARGIN;
        downButton_->setBounds(640, 385, BUTTON_WIDTH, BUTTON_HEIGHT);
        downButton_->setFontStyle(Font::BOLD);
        add(inputArea_.get());
        add(shareButton_.get());
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
        shareButton_->setEnabled(false);
        if (FacebookService::postFeed(inputArea_->getText())) {
           inputArea_->setText("");
        }
        shareButton_->setEnabled(true);
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
        if (event->getSource() == shareButton_.get()) {
            if (event->getType() == MouseEvent::MOUSE_RELEASED) {
                postFeed();
            }
        } else if (handleLikeButtonEvent(event)) {

        } else if (event->getSource() == downButton_.get()) {
            if (event->getType() == MouseEvent::MOUSE_RELEASED) {
                isAutoUpdate_ = false;
                setupFacebookPostViews(++offset_);
                uint64_t s = MonAPI::Date::nowInMsec();
                repaint();
                uint64_t e = MonAPI::Date::nowInMsec();
                logprintf("repaint=%d msec\n", e - s);
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

    void paintTitleGradation(Graphics* g)
    {
        int w = getWidth();
        dword c = g->getColor();
        g->setColor(0x29, 0x3e, 0x6a);
        g->fillRect(1, 0, w - 2, 1);
        g->setColor(0x6c, 0x83, 0xb2);
        g->fillRect(1, 1, w - 2, 1);
        g->setColor(0x3b, 0x59, 0x98);
        g->fillRect(1, 2, w - 2, 16);
        g->setColor(0x62, 0x7a, 0xad);
        g->fillRect(1, 18, w - 2, 2);
        g->setColor(0x89, 0x9b, 0xc1);
        g->fillRect(0, 20, w - 2, 1);
        g->setColor(0x29, 0x3e, 0x6b);
        g->fillRect(0, 21, w - 2, 1);
        g->setColor(c);
    }

    void paintTitleString(Graphics* g)
    {
        int w = getWidth();
        int fw = getFontMetrics()->getWidth(getTitle());
        int fh = getFontMetrics()->getHeight(getTitle());

        if (getFocused()) {
            g->setColor(monagui::Color::white);
        } else {
            g->setColor(monagui::Color::gray);
        }
        g->setFontStyle(Font::BOLD);
        g->drawString(getTitle(), ((w - fw) / 2), ((getInsets()->top - fh) / 2));
    }

    void paintFrame()
    {
        Graphics* g = getFrameGrapics();
        paintTitleGradation(g);
        drawCloseButton(g);
        paintTitleString(g);
    }

    void paint(Graphics *g)
    {
        g->setColor(getBackground());
        g->fillRect(0, 0, getWidth(), getHeight());

        paintFrame();

        g->setColor(monagui::Color::gray);
        g->drawLine(5, 9 + BUTTON_HEIGHT, getWidth(),  9 + BUTTON_HEIGHT);

        for (FacebookPostViews::const_iterator it = views_.begin(); it != views_.end(); ++it) {
            (*it)->draw(g);
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
    intptr_t ret = monapi_enable_stacktrace("/APPS/MONAGUI/FACEBOOK.MAP");
    if (ret != M_OK) {
        _printf("syscall_stack_trace_enable failed%d\n", ret);
    }

    uintptr_t updaterId = monapi_thread_create_with_arg(updaterLauncher, NULL);
    Facebook facebook(updaterId);
    facebook.run();
    return 0;
}
