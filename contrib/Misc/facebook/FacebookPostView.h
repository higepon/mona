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

typedef std::vector<Component*> Components;
class FacebookPostView
{
public:
    FacebookPostView(int x, int y, int w, int h) :
        x_(x),
        y_(y),
        likeButton_(new Button("Like!")),
        text_(new TextField()),
        image_(new WebImage()),
        postId_(""),
        numLikes_(0)
    {
        // todo w, h limit
        text_->setBounds(x + SIDE_BAR_WIDTH, y, w - MARGIN - LIKE_BUTTON_WIDTH, h);
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
        }
        text_->setText(content.c_str());
    }

    void setupFromFacebookPost(const FacebookPost& post)
    {
        setImagePath(post.imageUrl(), post.localImagePath());
        postId_ = post.postId;
        numLikes_ = post.numLikes;
        setText(post.text);
    }

    void setEmpty()
    {
        postId_ = "";
        numLikes_ = 0;
        setText("");
    }

    void drawImage(Graphics* g)
    {
        g->drawImage(image(), imageX(), imageY());
    }

    Button* likeButton()
    {
        return likeButton_.get();
    }

    void addLike()
    {
        ASSERT(!postId_.empty());
        FacebookService::addLike(postId_);
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
    MonAPI::scoped_ptr<Button> likeButton_;
    MonAPI::scoped_ptr<TextField> text_;
    MonAPI::scoped_ptr<WebImage> image_;
    std::string postId_;
    int numLikes_;
};

#endif // _FACEBOOK_POST_VIEW_
