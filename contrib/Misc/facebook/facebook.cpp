#include <monapi/StringHelper.h>
#include "FacebookFrame.h"
#include "ShareButton.h"
#include "Updater.h"
#include "FacebookService.h"
#include "FacebookPost.h"
#include "FacebookPostView.h"

using namespace std;
using namespace MonAPI;

typedef std::vector<FacebookPost> FacebookPosts;

class Facebook : public FacebookFrame {
 private:
     uintptr_t updaterId_;
     scoped_ptr<TextField> inputArea_;
     scoped_ptr<Button> shareButton_;
     scoped_ptr<Button> downButton_;
     scoped_ptr<Button> updateButton_;
     typedef std::vector<std::string> strings;

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
         FacebookFrame("Facebook"),
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
         setBackground(monagui::Color::white);
         setBounds(40, 40, WIDTH, HEIGHT);
         inputArea_->setBorderColor(0xffcccccc);
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
             if (line.size() == 7) {
                 Comments comments;
                 Strings cs = StringHelper::split(";", line[6]);
                 for (Strings::const_iterator it = cs.begin(); it != cs.end(); ++it) {
                     Strings idAndMessage = StringHelper::split(":", *it);
                     ASSERT(idAndMessage.size() == 2);
                     comments.push_back(Comment(idAndMessage[0], idAndMessage[1]));
                }
                 posts_.push_back(FacebookPost(line[0], line[1], line[2], atoi(line[3].c_str()), line[4], atoi(line[5].c_str()), comments));
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

     void paint(Graphics *g)
     {
         FacebookFrame::paint(g);
         g->setColor(getBackground());
         g->fillRect(0, 0, getWidth(), getHeight());
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



class CommentFrame : public FacebookFrame {
private:
    scoped_ptr<TextField> body_;
    std::string bodyText_;
public:
    CommentFrame(const std::string& postId) : FacebookFrame("Facebook comment"),
                                              body_(new TextField())
    {
        setBounds(80, 80, 300, 300);
        body_->setBounds(0, 0, 280, 280);
        add(body_.get());
        FacebookPosts posts;
        if (!readFacebookPostFromFile(posts)) {
            bodyText_ = "unknown post";
        }

        for (FacebookPosts::const_iterator it = posts.begin(); it != posts.end(); ++it) {
            if ((*it).postId == postId) {
                bodyText_ = (*it).text;
                return;
            }
        }
        bodyText_ = "unknown post";
    }

    bool readFacebookPostFromFile(FacebookPosts& posts)
    {
        scoped_ptr<SharedMemory> shm(monapi_file_read_all("/USER/TEMP/fb.data"));
        if (shm.get() == NULL) {
            return false;
        }
        std::string text((char*)shm->data());
        Strings lines = StringHelper::split("\n", text);
        for (size_t i = 0; i < lines.size(); i++) {
            Strings line = StringHelper::split("$", lines[i]);
            if (line.size() == 7) {
                Comments comments;
                Strings cs = StringHelper::split(";", line[6]);
                for (Strings::const_iterator it = cs.begin(); it != cs.end(); ++it) {
                    Strings idAndMessage = StringHelper::split(":", *it);
                    ASSERT(idAndMessage.size() == 2);
                    comments.push_back(Comment(idAndMessage[0], idAndMessage[1]));
                }
                posts.push_back(FacebookPost(line[0], line[1], line[2], atoi(line[3].c_str()), line[4], atoi(line[5].c_str()), comments));
            }
        }
        return true;
    }

     void paint(Graphics* g)
     {
         body_->setText(bodyText_.c_str());
         FacebookFrame::paint(g);
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
        monapi_fatal("syscall_stack_trace_enable failed%d\n", ret);
    }

    if (argc == 1) {
        uintptr_t updaterId = monapi_thread_create_with_arg(updaterLauncher, NULL);
        Facebook facebook(updaterId);
        facebook.run();
    } else {
        string postId(argv[1]);
        CommentFrame commentFrame(postId);
        commentFrame.run();
    }
    return 0;
}
