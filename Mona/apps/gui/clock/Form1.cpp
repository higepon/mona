// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.
// Higepon 2004

// This file's encoding is UTF-8.

#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Form.h>
#include <gui/System/Mona/Forms/Timer.h>
#include <gui/System/Mona/Forms/ControlPaint.h>
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

class Form1 : public Form
{
private:
    _P<Timer> timer1;
    char time[64];
    String clock;
    MonAPI::Date date;

public:
    Form1()
    {
        this->InitializeComponent();

        this->set_Size(Size(TEXT_LENGTH * FONT_WIDTH + 10, FONT_HEIGHT + 10));
        this->offset = Point::get_Empty();
        _P<MonAPI::Screen> scr = GetDefaultScreen();
        this->set_Location(Point(
            scr->getWidth () - this->get_Width(),
            scr->getHeight() - this->get_Height()));
        this->clock = this->getDateTime();
    }

private:
    void InitializeComponent()
    {
        this->set_Text("とけい");
        this->set_Opacity(0.8);

        this->set_ForeColor(Color::FromArgb( 76, 100, 112));
        this->set_BackColor(Color::FromArgb(128, 168, 189));

        this->timer1 = new Timer();
        this->timer1->set_Interval(300);
        this->timer1->add_Tick(new EventHandler<Form1>(this, &Form1::refresh));
    }

    void refresh(_P<Object> sender, _P<EventArgs> e)
    {
        String s = this->getDateTime();
        if (s == this->clock) return;

        this->clock = s;
        this->Refresh();
    }

    String getDateTime()
    {
        const char* day[] = { "日", "月", "火", "水", "木", "金", "土" };
        const char* ampm[] = { "午前", "午後" };

        this->date.refresh();
        sprintf(time, "%d年%02d月%02d日(%s) %s %02d:%02d:%02d",
            date.year(), date.month(), date.day(), day[date.dayofweek() % 7],
            ampm[date.hour() / 12], date.hour() % 12, date.min(), date.sec());
        return time;
    }

    void DrawString(_P<Graphics> g, String s, Color c, int pos_x, int pos_y)
    {
        _P<Font> f = Control::get_DefaultFont();
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
        END_FOREACH
    }

public:
    virtual void Create()
    {
        Form::Create();
        this->timer1->Start();
    }

    virtual void Dispose()
    {
        this->timer1->Dispose();
        Form::Dispose();
    }

protected:
    virtual void OnPaint()
    {
        _P<Graphics> g = this->CreateGraphics();
        int w = this->get_Width(), h = this->get_Height();

        // Clear
        g->FillRectangle(this->get_BackColor(), 0, 0, w, h);

        // Border
        ControlPaint::DrawEngraved(g, 0, 0, w, h);

        // Date
        this->DrawString(g, this->clock, this->get_ForeColor(), 5, 5);
        g->Dispose();
    }

    virtual NCState NCHitTest(int x, int y)
    {
        return NCState_TitleBar;
    }

    virtual void OnNCMouseDown(_P<MouseEventArgs> e)
    {
        if (e->Button != 2) Form::OnNCMouseDown(e);
    }

    virtual void OnNCMouseUp(_P<MouseEventArgs> e)
    {
        int x = e->X + this->offset.X, y = e->Y + this->offset.Y;
        if (e->Button == 2 && 0 <= x && x < this->get_Width() && 0 <= y && y < this->get_Height())
        {
            this->Dispose();
        }
        else
        {
            Form::OnNCMouseUp(e);
        }
    }

public:
    static void Main(_A<String> args)
    {
        if (ExistsProcess("CLOCK.EL2")) return;

        Application::Run(new Form1());
    }
};

SET_MAIN_CLASS(Form1)
