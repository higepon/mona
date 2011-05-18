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

class FacebookPostView : public Container
{
public:
    FacebookPostView(int x, int y, int w, int h) :
        likeButton_(new Button("いいね!")),
        text_(new TextField()),
        image_(new WebImage())
    {
        setBounds(x, y, w, h);
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
    MonAPI::scoped_ptr<Button> likeButton_;
    MonAPI::scoped_ptr<TextField> text_;
    MonAPI::scoped_ptr<WebImage> image_;
};

#endif // _FACEBOOK_POST_VIEW_
