/*
 *
 *   Copyright (c) 2011  Higepon(Taro Minowa)  <higepon@users.sourceforge.jp>
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef _FACEBOOK_POST_VIEW_
#define _FACEBOOK_POST_VIEW_

class FBButton : public Button
{
public:
    FBButton(const char* title) : Button(title)
    {
        setBackground(monagui::Color::white);
        setForeground(0xff6d84b4);
    }

    virtual ~FBButton()
    {
    }

    void paint(Graphics* g)
    {
        int w = getWidth();
        int h = getHeight();

        g->setColor(getBackground());
        g->fillRect(0, 0, w, h);

        if (getPushed()) {
            g->setColor(Color::white);
            g->drawLine(2, h - 2, w - 3, h - 2);
            g->drawLine(w - 2, 2, w - 2, h - 3);
            g->drawLine(w - 3 , h - 3, w - 3, h - 3);
            g->setColor(Color::gray);
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

        g->setColor(getForeground());
        g->drawString(getLabel(), x, y);
    }
};

typedef std::vector<Component*> Components;
class FacebookPostView
{
public:
    FacebookPostView(int x, int y, int w, int h) :
        x_(x),
        y_(y),
        w_(w),
        h_(h),
        likeButton_(new FBButton("Like")),
        text_(new TextField()),
        image_(new WebImage()),
        postId_(""),
        numLikes_(0),
        numComments_(0)
    {
        // todo w, h limit
        text_->setBounds(x + SIDE_BAR_WIDTH, y, w - MARGIN - LIKE_BUTTON_WIDTH, h - 5);
        text_->setForeground(monagui::Color::black);
        text_->setBorderColor(monagui::Color::white);
        likeButton_->setBackground(monagui::Color::white);
        likeButton_->setForeground(0xff6d84b4);
        likeButton_->setBounds(x, y + IMAGE_HEIGHT + IMAGE_MARGIN_TOP + LIKE_BUTTON_MARGIN_TOP, LIKE_BUTTON_WIDTH, LIKE_BUTTON_HEIGHT);
    }

    virtual ~FacebookPostView()
    {
    }

    void components(Components& ret)
    {
        ret.push_back(likeButton_.get());
        ret.push_back(text_.get());
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
        std::string content = foldLine(text, 70);
        if (numLikes_ > 0) {
            content += "\n";
            char buf[32];
            sprintf(buf, "%d", numLikes_);
            content += buf;
            content += "人がいいね！と言っています。";

            sprintf(buf, "%d", numComments_);
            content += buf;
            content += "個のコメント";
        }
        text_->setText(content.c_str());
    }

    void setupFromFacebookPost(const FacebookPost& post)
    {
        setImagePath(post.imageUrl(), post.localImagePath());
        postId_ = post.postId;
        numLikes_ = post.numLikes;
        numComments_ = post.numComments;
        setText(post.text);
    }

    void setEmpty()
    {
        postId_ = "";
        numLikes_ = 0;
        setText("");
    }

    void draw(Graphics* g)
    {
        g->drawImage(image(), imageX(), imageY());
        dword c = g->getColor();
        g->setColor(monagui::Color::gray);
        g->drawLine(x_, y_ + h_ - 2, x_ + w_, y_ + h_ - 2);
        g->setColor(c);
    }

    Button* likeButton()
    {
        return likeButton_.get();
    }

    void addLike()
    {
        if (!postId_.empty()) {
            FacebookService::addLike(postId_);
        }
    }

    std::string foldLine(const std::string& line, size_t maxLineLength)
    {
        size_t len = 0;
        std::string ret;
        for (std::string::const_iterator it = line.begin(); it != line.end(); ++it) {
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

private:

    Image* image()
    {
        return image_.get();
    }

    int imageX() const
    {
        return x_;
    }

    int imageY() const
    {
        return y_ + IMAGE_MARGIN_TOP;
    }

    bool isImageValid() const
    {
        return image_->getWidth() != 0;
    }

    enum
    {
        MARGIN = 25,
        IMAGE_MARGIN_TOP = 5,
        IMAGE_HEIGHT = 20,
        IMAGE_WIDTH = 20,
        LIKE_BUTTON_WIDTH = 40,
        LIKE_BUTTON_HEIGHT = 20,
        LIKE_BUTTON_MARGIN_TOP = 2,
        SIDE_BAR_WIDTH = LIKE_BUTTON_WIDTH
    };
    int x_;
    int y_;
    int w_;
    int h_;
    MonAPI::scoped_ptr<Button> likeButton_;
    MonAPI::scoped_ptr<TextField> text_;
    MonAPI::scoped_ptr<WebImage> image_;
    std::string postId_;
    int numLikes_;
    int numComments_;
};

#endif // _FACEBOOK_POST_VIEW_
