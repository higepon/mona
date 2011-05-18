#include <baygui.h>
#include <string>
#include <monapi/StringHelper.h>
#include "Updater.h"

using namespace std;
using namespace MonAPI;

struct FacebookPost
{
    FacebookPost(const std::string& imageId,
                 const std::string& name,
                 const std::string& text,
                 uint32_t numLikes,
                 const std::string& postId
        ) :
        imageId(imageId),
        name(name),
        text(text),
        numLikes(numLikes),
        postId(postId)
    {
    }

    std::string imageUrl()
    {
        std::string ret = "http://graph.facebook.com/";
        ret += imageId;
        ret += "/picture";
        return ret;
    }

    std::string localImagePath()
    {
        std::string ret = "/USER/TEMP/" + imageId + ".JPG";
        return ret;
    }

    std::string imageId;
    std::string name;
    std::string text;
    uint32_t numLikes;
    std::string postId;
};

class FacebookPostView : public Container
{
public:
    FacebookPostView() :
        likeButton_(new Button("いいね!")),
        text_(new TextField()),
        image_(new WebImage())
    {
        text_->setBounds(getX() + SIDE_BAR_WIDTH, getY() + 0, TEXT_FIELD_WIDTH, HEIGHT);
        likeButton_->setBounds(getX() + 0, getY() + IMAGE_HEIGHT, LIKE_BUTTON_WIDTH, LIKE_BUTTON_HEIGHT);
        add(text_.get());
        add(likeButton_.get());
    }

    virtual ~FacebookPostView()
    {
    }

    void setImagePath(const std::string& uri, const std::string& path)
    {
        image_->initialize(uri, path);
        if (isImageValid()) {
            image_->resize(IMAGE_WIDTH, IMAGE_HEIGHT);
        }
    }

    void setText(const std::string& text)
    {
        text_->setText(text.c_str());
    }

private:

    bool isImageValid() const
    {
        return image_->getWidth() != 0;
    }


    void repaint()
    {
        if (isImageValid()) {
            getGraphics()->drawImage(image_.get(), 0, 0);
        }

        Container::repaint();
    }

    enum
    {
        SIDE_BAR_WIDTH = 50,
        HEIGHT = 50,
        IMAGE_HEIGHT = 20,
        IMAGE_WIDTH = 20,
        LIKE_BUTTON_WIDTH = 40,
        LIKE_BUTTON_HEIGHT = 20,
        TEXT_FIELD_WIDTH = 600
    };
    scoped_ptr<Button> likeButton_;
    scoped_ptr<TextField> text_;
    scoped_ptr<WebImage> image_;
};

class FacebookService
{
public:
    static bool postFeed(const std::string& text)
    {
        return executeMosh("/LIBS/MOSH/bin/fb-feed-post.sps", text);
    }

    static bool addLike(const std::string& postId)
    {
        return executeMosh("/LIBS/MOSH/bin/fb-like-post.sps", postId);
    }

private:
    static bool executeMosh(const std::string& script, const std::string& arg)
    {
       uint32_t tid;
        std::string command(System::getMoshPath());
        command += " ";
        command += script;
        command += " ";
        command += arg;
        int result = monapi_process_execute_file_get_tid(command.c_str(),
                                                              MONAPI_TRUE,
                                                              &tid,
                                                              System::getProcessStdinID(),
                                                              System::getProcessStdoutID());
        if (result != 0) {
            monapi_fatal("can't exec Mosh");
        }
        if (0 == monapi_process_wait_terminated(tid)) {
            return true;
        } else {
            return false;
        }
    }
};



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

        // for (size_t i = 0; i < MAX_ROWS; i++) {
        //     TextField* field = new TextField();
        //     fields_.push_back(field);
        //     field->setBounds(IMAGE_WIDTH, 50 + IMAGE_HEIGHT * i, WIDTH - IMAGE_WIDTH - MARGIN, IMAGE_HEIGHT);
        //     add(field);

        //     Button* button = new Button("いいね！");
        //     likeButtons_.push_back(button);
        //     button->setBounds(0, 50 + IMAGE_HEIGHT * i + 20, 40, 20);
        //     add(button);
        // }

        for (size_t i = 0; i < MAX_ROWS; i++) {
            FacebookPostView* view = new FacebookPostView();
            views_.push_back(view);
            view->setBounds(IMAGE_WIDTH, 50 + IMAGE_HEIGHT * i, WIDTH - IMAGE_WIDTH - MARGIN, IMAGE_HEIGHT);
            add(view);
        }
    }

    ~Display()
    {
        disposeImages();
        disposeLikeButtons();
    }

private:
    uintptr_t updaterId_;
    scoped_ptr<TextField> inputArea_;
    scoped_ptr<Button> postButton_;
    scoped_ptr<Button> downButton_;
    scoped_ptr<Button> updateButton_;
//    typedef std::vector<TextField*> TextFields;
//    typedef std::vector<Image*> Images;
//    typedef std::vector<Button*> Buttons;
    typedef std::vector<std::string> strings;
    typedef std::vector<FacebookPost> FacebookPosts;
    typedef std::vector<FacebookPostView*> FacebookPostViews;
    FacebookPosts posts_;
//    TextFields fields_;
//    Images images_;
//    Buttons likeButtons_;
    bool updating_;
    int idleTimeMsec_;
    int offset_;
    FacebookPostViews views_;

    void disposeImages()
    {
        // for (Images::const_iterator it = images_.begin(); it != images_.end(); ++it) {
        //     delete (*it);
        // }
        // images_.clear();
    }

    void disposeLikeButtons()
    {
        // for (Buttons::const_iterator it = likeButtons_.begin(); it != likeButtons_.end(); ++it) {
        //     delete (*it);
        // }
        // likeButtons_.clear();
    }


    void disposeTextFields()
    {
        // for (TextFields::const_iterator it = fields_.begin(); it != fields_.end(); ++it) {
        //     remove(*it);
        //     delete (*it);
        // }
        // fields_.clear();
    }

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
//        disposeImages();
//        disposeTextFields();

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
        // for (size_t i = 0; i < images_.size(); i++) {
        //     g->drawImage(images_[i], 0, IMAGE_HEIGHT * i + 50);
        // }
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
