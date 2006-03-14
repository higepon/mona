#ifndef __TOPLABEL_H__
#define __TOPLABEL_H__

#include <baygui.h>
#include <vector>
#include <string>

namespace baygui {

class TopLabel : public Label
{
public:
    TopLabel();
    virtual ~TopLabel();
    TopLabel(const String& text, int align, int fontHeight);

    void setText(const String& text);
    String getText(int startLine = 0);
    void scrollDown();
    void scrollUp();
    bool needScrollUp();
    bool needScrollDown();
    unsigned int getExtraLineNumber();

private:
    virtual void paint(Graphics* g);

protected:
    int countLine(std::string text);
    String getSubString(int startLine);

    int fontHeight;
    int align;
    int virtualHeight;
    int startLine;
    unsigned int maxLines;
    std::vector<std::string> lines;

};

}; // namespace baygui

#endif // __TOPLABEL_H__
