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

#ifndef MONA_FACEBOOK_FEED_VIEW_H
#define MONA_FACEBOOK_FEED_VIEW_H

#include <monagui.h>
#include "feed.h"

namespace facebook {

typedef std::vector<Component*> Components;

class FeedView
{
public:
    FeedView(int x, int y, int w, int h);
    virtual ~FeedView();

    void components(Components& ret);
    void setImagePath(const std::string& uri, const std::string& path);
    void setText(const std::string& text);
    void setupFromFeed(const Feed& feed);
    void setEmpty();
    void draw(Graphics* g);

    Button* likeButton()
    {
        return likeButton_.get();
    }

    Button* commentButton()
    {
        return commentButton_.get();
    }

    void openComment();
    void addLike();
    std::string foldLine(const std::string& line, size_t maxLineLength);

private:

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
    MonAPI::scoped_ptr<Button> commentButton_;
    MonAPI::scoped_ptr<TextField> text_;
    MonAPI::scoped_ptr<ImageIcon> icon_;
    std::string postId_;
    int numLikes_;
    int numComments_;
    Comments comments_;

    DISALLOW_COPY_AND_ASSIGN(FeedView);
};

}

#endif // MONA_FACEBOOK_FEED_VIEW_H
