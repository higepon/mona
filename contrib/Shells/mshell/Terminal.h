#ifndef _TERMINAL_
#define _TERMINAL_

#include <monapi/terminal/Writer.h>
#include <monapi/text/Line.h>
#include <monapi/CString.h>
#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Button.h>
#include <gui/System/Mona/Forms/Form.h>
#include <gui/System/Mona/Forms/ControlPaint.h>

class Terminal : public System::Mona::Forms::Control, public MonAPI::terminal::Writer
{
public:
    Terminal();
    virtual ~Terminal();

    // MonAPI::terminal::Writer interfaces
    virtual int clearScreen();
    virtual int moveCursor(uint32_t x, uint32_t y);
    virtual int moveCursorUp(uint32_t n);
    virtual int moveCursorDown(uint32_t n);
    virtual int moveCursorLeft(uint32_t n);
    virtual int moveCursorRight(uint32_t n);
    virtual int lineFeed();
    virtual int backSpace();
    virtual int write(uint8_t* buf, uint32_t length);
    virtual uint32_t getX() const { return x_; }
    virtual uint32_t getY() const { return y_; }

    virtual void OnPaint();
    virtual int eraseCursor(_P<System::Drawing::Graphics> g);
    virtual int drawLine(_P<System::Drawing::Graphics> g);

    enum
    {
        FONT_WIDTH  = 6,
        FONT_HEIGHT = 12,
        WIDTH       = 80,
        HEIGHT      = 25
    };

private:
    int x_, y_;
    MonAPI::text::Line line_;
    int lastCursorX_;
    bool first_;
    System::Drawing::Color forgroundColor_;
    System::Drawing::Color backgroundColor_;
};

#endif // _TERMINAL_
