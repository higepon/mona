#include <monapi/text/Line.h>

using namespace MonAPI;
using namespace MonAPI::text;

Line::Line() : position_(0)
{
}

Line::~Line()
{
}

CString Line::get()
{
    return text_;
}

void Line::write(const CString& text)
{
    int currentLength = text_.getLength();

    if (currentLength == position_)
    {
        text_ += text;
        position_ += text.getLength();
    }
    else
    {
        text_ = text_.substring(0, position_);
        text_ += text;
        position_ = text_.getLength();
    }
}

int Line::getCursorPosition()
{
    return position_;
}

void Line::moveCursorLeft()
{
    position_--;
    if (position_ <0) position_ = 0;
    return;
}

void Line::moveCursorRight()
{
    position_++;
    if (position_ >= text_.getLength()) position_ = text_.getLength();
    return;
}

void Line::reset()
{
    position_ = 0;
    text_ = "";
}
