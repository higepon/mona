// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Form.h>
#include <gui/System/Mona/Forms/Label.h>

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

class Form1 : public Form
{
public:
    _P<Label> label1;
    String aa[15];

    Form1()
    {
        this->InitializeComponent();
        this->aa[14] = " ⊂(ﾟーﾟ*⊂⌒｀つ≡≡≡≡≡≡≡≡≡";
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

    void animation()
    {
        int i = 0;

        this->Show();
        for (; this->visible; Application::DoEvents())
        {
            this->label1->set_Text(aa[(i++) & 0xF]);
            this->OnPaint();
            sleep(300);
        }
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
    }

public:
    static void Main(_A<String> args)
    {
        Form1* form = new Form1();
        form->animation();
    }
};

SET_MAIN_CLASS(Form1)
