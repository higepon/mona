// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.
// Higepon 2004

// This file's encoding is UTF-8.

#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Form.h>
#include <gui/System/Mona/Forms/Label.h>
#include <gui/System/Mona/Forms/Timer.h>

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

class Form1 : public Form
{
private:
    _P<Label> label1;
    _P<Timer> timer1;
    char time[64];
    MonAPI::Date date;

public:
    Form1()
    {
        this->InitializeComponent();
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
        char* day[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
        this->date.refresh();

        sprintf(time, "%d/%02d/%02d (%s)%02d:%02d:%02d", date.year(), date.month(), date.day(), day[date.dayofweek()], date.hour(), date.min(), date.sec());

        this->label1->set_Text(time);
    }

private:
    void InitializeComponent()
    {
        this->label1 = new Label();

        this->set_Location(Point(100, 100));
        this->set_ClientSize(Size(140, 30));
        this->set_Text("とけい");

        this->label1->set_Bounds(Rectangle(5, 10, 130, 25));
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
        Application::Run(new Form1());
    }
};

SET_MAIN_CLASS(Form1)
