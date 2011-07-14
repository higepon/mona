#include <monapi/StringHelper.h>
#include "frame.h"
#include "button.h"
#include "share_button.h"
#include "updater.h"
#include "facebook_service.h"
#include "feed_view.h"
#include "parser.h"

using namespace std;
using namespace MonAPI;
using namespace facebook;

class Facebook : public facebook::Frame {
private:
    uintptr_t updaterId_;
    scoped_ptr<TextField> inputArea_;
    scoped_ptr<facebook::Button> shareButton_;
    scoped_ptr<facebook::Button> downButton_;
    scoped_ptr<facebook::Button> updateButton_;
     typedef std::vector<std::string> strings;

     typedef std::vector<FeedView*> FeedViews;
     Feeds feeds_;
     bool updating_;
     int idleTimeMsec_;
     int offset_;
     FeedViews views_;
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
         facebook::Frame("Facebook"),
         updaterId_(updaterId),
         inputArea_(new TextField()),
         shareButton_(new ShareButton()),
         downButton_(new facebook::Button(">>>")),
         updateButton_(new facebook::Button("Update")),
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
             FeedView* view = new FeedView(5, y + POST_HEIGHT * i, WIDTH, POST_HEIGHT);
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

    bool readFeedFromFile()
    {
        Parser parser("/USER/TEMP/fb.json");
        bool ret = parser.parse(feeds_);
        if (!ret) {
            logprintf("err=%s\n", parser.lastError().c_str());
        }
        return ret;
     }

     void show()
     {
         if (!readFeedFromFile()) {
             monapi_fatal("can't read fb.data");
         }
         setupFeedViews();
     }

     void setupFeedViews(size_t offset = 0)
     {
         for (size_t i = 0; i < MAX_ROWS; i++) {
             //uint64_t s1 = MonAPI::Date::nowInMsec();
             if (i + offset < feeds_.size()) {
                 views_[i]->setupFromFeed(feeds_[i + offset]);
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
         for (FeedViews::const_iterator it = views_.begin(); it != views_.end(); ++it) {
             if ((*it)->likeButton() == event->getSource()) {
                 (*it)->addLike();
                 updateFeedAsync();
                 return true;
             } else if ((*it)->commentButton() == event->getSource()) {
                 (*it)->openComment();
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
                 setupFeedViews(++offset_);
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
         facebook::Frame::paint(g);
         g->setColor(monagui::Color::gray);
         g->drawLine(5, 9 + BUTTON_HEIGHT, getWidth(),  9 + BUTTON_HEIGHT);

         for (FeedViews::const_iterator it = views_.begin(); it != views_.end(); ++it) {
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

class CommentFrame : public facebook::Frame {
private:
    scoped_ptr<TextField> body_;
    scoped_ptr<TextField> likes_;
    scoped_ptr<TextField> commentInput_;
    scoped_ptr<ShareButton> commentButton_;
    Comments comments_;
    std::vector<TextField*> commentFields_;
    scoped_ptr<WebImage> iconImage_;
    scoped_ptr<ImageIcon> icon_;
    std::string postId_;
public:

    CommentFrame(const std::string& postId) : facebook::Frame("Facebook comment"),
                                              body_(new TextField()),
                                              likes_(new TextField()),
                                              commentInput_(new TextField()),
                                              commentButton_(new ShareButton("Post")),
                                              iconImage_(new WebImage()),
                                              icon_(new ImageIcon(iconImage_.get())),
                                              postId_("")
    {
        const int WIDTH = 300;
        setBackground(monagui::Color::white);
        body_->setBorderColor(monagui::Color::white);
        body_->setEditable(false);
        add(body_.get());

        likes_->setBackground(0xffedeff4);
        likes_->setBorderColor(0xffedeff4);
        likes_->setForeground(0xff293e6a);
        likes_->setEditable(false);
        add(likes_.get());

        const int ICON_MARGIN = 5;
        const int ICON_SIZE = 30;
        icon_->setBounds(ICON_MARGIN, ICON_MARGIN, ICON_SIZE, ICON_SIZE);
        add(icon_.get());
        Feeds posts;
        if (!readFeedFromFile(posts)) {
            body_->setTextNoRepaint("unknown post");
        }

        int componentY = 0;
        // TODO: image, field, destruct.
        for (Feeds::const_iterator it = posts.begin(); it != posts.end(); ++it) {
            if ((*it).postId == postId) {
                postId_ = postId;
                iconImage_->initialize((*it).profileImageUrl(), (*it).localImagePath());

                // message body
                const int BODY_MINIMUM_HEIGHT = ICON_SIZE + 10;
                const int BODY_RIGHT_MARGIN = 20;
                const int BODY_MAX_WIDTH = WIDTH - ICON_MARGIN * 2 - ICON_SIZE - BODY_RIGHT_MARGIN;
                int bodyHeight = body_->getHeightByTextAndMaxWidth((*it).text.c_str(), BODY_MAX_WIDTH);
                bodyHeight = bodyHeight < BODY_MINIMUM_HEIGHT ? BODY_MINIMUM_HEIGHT : bodyHeight;
                body_->setBounds(ICON_MARGIN * 2 + ICON_SIZE, componentY, BODY_MAX_WIDTH, bodyHeight);
                body_->setTextNoRepaint((*it).text.c_str());
                componentY += bodyHeight;

                // likes
                if ((*it).numLikes > 0) {
                    add(likes_.get());
                    componentY += ICON_MARGIN;
                    const int LIKES_HEIGHT = 25;
                    likes_->setBounds(ICON_MARGIN, componentY, WIDTH - 25, LIKES_HEIGHT);
                    componentY += LIKES_HEIGHT;
                    char buf[64];
                    sprintf(buf, "%d人", (*it).numLikes);
                    likes_->setTextNoRepaint(buf);
                }
                int i = 0;
                comments_ = (*it).comments;
                for (Comments::const_iterator it = comments_.begin(); it != comments_.end(); ++it) {
                    WebImage* commentIconImage = new WebImage();
                    ImageIcon* commentIcon = new ImageIcon(commentIconImage);
                    std::string imageUrl("http://graph.facebook.com/");
                    imageUrl += (*it).id;
                    imageUrl += "/picture";
                    std::string localImagePath("/USER/TEMP/");
                    localImagePath += (*it).id;
                    localImagePath += ".JPG";
                    commentIcon->setBounds(ICON_MARGIN, componentY, ICON_SIZE, ICON_SIZE);
                    add(commentIcon);
                    commentIconImage->initialize(imageUrl, localImagePath);
                    TextField* textField = new TextField;
                    const int COMMENT_WIDTH = WIDTH - ICON_MARGIN * 2 - ICON_SIZE - 20;
                    const int COMMENT_MINIMUM_HEIGHT = ICON_SIZE + 10;
                    int commentHeight = body_->getHeightByTextAndMaxWidth((*it).body.c_str(), COMMENT_WIDTH);
                    commentHeight = commentHeight < COMMENT_MINIMUM_HEIGHT ? COMMENT_MINIMUM_HEIGHT : commentHeight;
                    textField->setTextNoRepaint((*it).body.c_str());
                    textField->setBounds(ICON_SIZE + ICON_MARGIN * 2, componentY, COMMENT_WIDTH, commentHeight);
                    componentY += commentHeight;
                    textField->setBackground(0xffedeff4);
                    textField->setBorderColor(0xffedeff4);
//                    textField->setBorderColor(0xffffffff);
//                    textField->setBorderColor(monagui::Color::black);
                    textField->setEditable(false);
                    add(textField);
                    i++;
                }
                commentInput_->setBorderColor(0xffcccccc);
                commentInput_->setBounds(0, componentY, WIDTH - 50, 20);
                commentButton_->setBounds(WIDTH - 50, componentY, 30, 20);
                componentY += 100;
                add(commentInput_.get());
                add(commentButton_.get());
                const int MINIMUM_WINDOW_HEIGHT = 300;
                componentY = componentY < MINIMUM_WINDOW_HEIGHT ? MINIMUM_WINDOW_HEIGHT : componentY;
                setBounds(80, 80, WIDTH, componentY);
                return;
            }
        }
        body_->setTextNoRepaint("unknown post");

    }

     void processEvent(Event* event)
     {
         if (event->getSource() == commentButton_.get() &&
             event->getType() == MouseEvent::MOUSE_RELEASED) {
             string message(commentInput_->getText());
             if (FacebookService::postComment(postId_, message)) {
                 commentInput_->setText("comment published");
             } else {
                 message += " : comment publis error";
                 commentInput_->setText(message.c_str());
             }
        }
    }


    bool readFeedFromFile(Feeds& feeds)
    {
        Parser parser("/USER/TEMP/fb.json");
        bool ret = parser.parse(feeds);
        if (!ret) {
            logprintf("err=%s\n", parser.lastError().c_str());
        }
        return ret;
    }

     void paint(Graphics* g)
     {
         facebook::Frame::paint(g);
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
