// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.
// Higepon 2004

// This file's encoding is UTF-8.

#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Form.h>
#include <gui/System/Mona/Forms/Timer.h>
#include <monapi/CString.h>

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

#define FONT_WIDTH    6
#define FONT_HEIGHT  12
#define TEXT_LENGTH  32

extern _P<MonAPI::Screen> GetDefaultScreen();

static bool ExistsProcess(const MonAPI::CString& self)
{
    syscall_set_ps_dump();
    PsInfo info;

    bool ret = false;
    dword tid = syscall_get_tid();

    while (syscall_read_ps_dump(&info) == 0)
    {
        if (!ret && self == info.name && tid != info.tid) ret = true;
    }
    if (ret) printf("%s: already has executed!\n", (const char*)self);

    return ret;
}

class FixedLabel : public Control
{
public:
    FixedLabel() {}
    virtual ~FixedLabel() {}

    static void DrawString(_P<Graphics> g, String s, _P<Font> f, Color c, int pos_x, int pos_y)
    {
        int x = pos_x, y = pos_y;
        FOREACH (wchar, ch, s)
        {
            if (ch == '\n')
            {
                x = pos_x;
                y += FONT_HEIGHT;
            }
            else
            {
                _A<wchar> buf(1);
                buf[0] = ch;
                int sw = g->MeasureString(buf, Control::get_DefaultFont()).Width;
                int sw2 = ch < 256 ? FONT_WIDTH : FONT_WIDTH * 2;
                g->DrawString(buf, Control::get_DefaultFont(), Color::get_Black(), x + (sw2 - sw) / 2, y);
                x += sw2;
            }
        }
    }

protected:
    virtual void OnPaint()
    {
        _P<Graphics> g = this->CreateGraphics();
        g->FillRectangle(this->get_BackColor(), 0, 0, this->get_Width(), this->get_Height());
        FixedLabel::DrawString(g, this->get_Text(), Control::get_DefaultFont(), this->get_ForeColor(), 0, 0);
        g->Dispose();
    }
};

class Form1 : public Form
{
private:
    _P<FixedLabel> label1;
    _P<Timer> timer1;
    MonAPI::Date date;

public:
    Form1()
    {
        this->InitializeComponent();

        _P<MonAPI::Screen> scr = GetDefaultScreen();
        this->set_Location(Point(
            scr->getWidth () - this->get_Width(),
            scr->getHeight() - this->get_Height()));
    }

    virtual void Create()
    {
        Form::Create();
        this->timer1->Start();
    }

    virtual void Dispose()
    {
        Form::Dispose();
        this->timer1->Dispose();
    }

    void refresh(_P<Object> sender, _P<EventArgs> e)
    {
        const char* day[] = { "日", "月", "火", "水", "木", "金", "土" };
        const char* ampm[] = { "午前", "午後" };

        this->date.refresh();
        char time[64];
        sprintf(time, "%d年%02d月%02d日(%s) %s %02d:%02d:%02d",
            date.year(), date.month(), date.day(), day[date.dayofweek() % 7],
            ampm[date.hour() / 12], date.hour() % 12, date.min(), date.sec());
        this->label1->set_Text(time);
    }

private:
    void InitializeComponent()
    {
        this->label1 = new FixedLabel();

        this->set_ClientSize(Size(TEXT_LENGTH * FONT_WIDTH + 10, 30));
        this->set_Text("とけい");

        this->label1->set_Bounds(Rectangle(5, 10, TEXT_LENGTH * FONT_WIDTH, 25));
        this->get_Controls()->Add(this->label1.get());

        this->set_ForeColor(Color::FromArgb(100, 112, 76));
        this->set_BackColor(Color::FromArgb(168, 189, 128));

        this->timer1 = new Timer();
        this->timer1->set_Interval(300);
        this->timer1->add_Tick(new EventHandler<Form1>(this, &Form1::refresh));
    }

public:
    static void Main(_A<String> args)
    {
        if (ExistsProcess("CLOCK.EL2")) return;

        Application::Run(new Form1());
    }
};

SET_MAIN_CLASS(Form1)
