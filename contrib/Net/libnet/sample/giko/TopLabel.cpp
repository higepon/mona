#include "TopLabel.h"
#include "StringHelper.h"
#include <string>

using namespace baygui;
using namespace std;

TopLabel::TopLabel()
{
}

TopLabel::~TopLabel()
{
}

TopLabel::TopLabel(const String& text, int align, int fontHeight) : Label(text, align)
{
    this->fontHeight = fontHeight;
    this->startLine = 0;
    setText(text);
}

void TopLabel::setText(const String& text)
{
    this->lines = StringHelper::split((const char*)text);
    this->maxLines = this->getHeight() / this->fontHeight;

    repaint();
}

String TopLabel::getText(int startLine /* = 0 */)
{
    string result;
    for (unsigned int i = startLine; i < this->lines.size(); i++)
    {
        result += this->lines[i] + "\n";
    }
    return result.c_str();
}

bool TopLabel::needScrollUp()
{
    if (this->startLine == 0) return false;
    return true;
}

bool TopLabel::needScrollDown()
{
    if (this->maxLines >= this->lines.size()) return false;
    if (this->lines.size() - this->startLine <= this->maxLines) return false;
    return true;
}

unsigned int TopLabel::getExtraLineNumber()
{
    if (this->maxLines >= this->lines.size())
    {
        return 0;
    }
    return this->lines.size() - this->maxLines;
}

void TopLabel::scrollDown()
{
    if (needScrollDown())
    {
        this->startLine++;
        repaint();
    }
}

void TopLabel::scrollUp()
{
    if (needScrollUp())
    {
        this->startLine--;
        repaint();
    }
}


// I wanna valign=top !
void TopLabel::paint(Graphics* g)
{
    int w = getWidth();
    int h = getHeight();
    g->setColor(getBackground());
    g->fillRect(0, 0, w, h);

    int fw = getFontMetrics()->getWidth(getText());
    if (getEnabled() == true)
    {
        g->setColor(getForeground());
    }
    else
    {
        g->setColor(Color::gray);
    }

    String text = getText(startLine);
    if (this->align == Label::RIGHT)
    {
        g->drawString(text, (w - fw), 0);
    }
    else if (this->align == Label::CENTER)
    {
        g->drawString(text, (w - fw) / 2, 0);
    }
    else
    {
        g->drawString(text, 0, 0);
    }
}
