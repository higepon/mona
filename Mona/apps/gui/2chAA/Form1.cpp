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
    _A<String> aa;
    int count;

public:
    Form1() : count(0)
    {
        aa.Alloc(16);
        this->InitializeComponent();
        this->aa[15] = "⊂(ﾟーﾟ*⊂⌒｀つ≡≡≡≡≡≡≡≡≡ ";
        this->aa[14] = " ⊂(ﾟーﾟ*⊂⌒｀つ≡≡≡≡≡≡≡≡≡";
        this->aa[13] = "   ⊂(ﾟーﾟ*⊂⌒｀つ≡≡≡≡≡≡≡≡";
        this->aa[12] = "     ⊂(ﾟーﾟ*⊂⌒｀つ≡≡≡≡≡≡≡";
        this->aa[11] = "         ⊂(ﾟーﾟ*⊂⌒｀つ≡≡≡≡≡";
        this->aa[10] = "           ⊂(ﾟーﾟ*⊂⌒｀つ≡≡≡≡";
        this->aa[9]  = "             ⊂(ﾟーﾟ*⊂⌒｀つ≡≡≡";
        this->aa[8]  = "               ⊂(ﾟーﾟ*⊂⌒｀つ≡≡";
        this->aa[7]  = "                 ⊂(ﾟーﾟ*⊂⌒｀つ≡";
        this->aa[6]  = "                   ⊂(ﾟーﾟ*⊂⌒｀つ";
        this->aa[5]  = "                     ⊂(ﾟーﾟ*⊂⌒｀";
        this->aa[4]  = "                       ⊂(ﾟーﾟ*⊂⌒";
        this->aa[3]  = "                         ⊂(ﾟーﾟ*⊂";
        this->aa[2]  = "                             ⊂(ﾟー";
        this->aa[1]  = "                                ⊂*";
        this->aa[0]  = "                                   ";
    }

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

    void animation(_P<Object> sender, _P<EventArgs> e)
    {
        this->label1->set_Text(aa[(this->count++) % aa.get_Length()]);
    }

private:
    void InitializeComponent()
    {
        this->label1 = new Label();

        this->set_Location(Point(50, 50));
        this->set_ClientSize(Size(150, 50));
        this->set_Text("ズサー");

        this->label1->set_Bounds(Rectangle(0, 20, 140, 40));
        this->label1->set_Text(aa[0]);
        this->get_Controls()->Add(this->label1.get());

        this->timer1 = new Timer();
        this->timer1->set_Interval(300);
        this->timer1->add_Tick(new EventHandler<Form1>(this, &Form1::animation));
    }

public:
    static void Main(_A<String> args)
    {
        Application::Run(new Form1());
    }
};

SET_MAIN_CLASS(Form1)
