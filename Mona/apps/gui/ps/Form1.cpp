#include <monapi.h>
#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Form.h>
#include <gui/System/Mona/Forms/Label.h>

/* higepon */

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

class Label1 : public Label
{
public:
    Label1() {};
    virtual ~Label1() {};

protected:
    virtual void OnPaint()
    {
        _P<Graphics> g = this->CreateGraphics();
        g->FillRectangle(Color::FromArgb(168, 189, 128), 0, 0, get_Width(), get_Height());
        g->DrawString(this->get_Text(), Control::get_DefaultFont(), Color::FromArgb(100, 112, 76), 0, 0);
        g->Dispose();
    }
};

class Form1 : public Form
{
public:
    Pointer<Label1> label0;
    Pointer<Label1> label1;
    Pointer<Label1> label2;
    Pointer<Label1> label3;
    Pointer<Label1> label4;
    Pointer<Label1> label5;
    Pointer<Label1> label6;

    char format[256];
    char message1[256];
    char message2[256];
    char message3[256];
    char message4[256];
    char message5[256];
    char message6[256];

    Form1()
    {
        *message1 = '\0';
        *message2 = '\0';
        *message3 = '\0';
        *message4 = '\0';
        *message5 = '\0';
        *message6 = '\0';
        this->InitializeComponent();
    }

private:
    void InitializeComponent()
    {
        this->label0 = new Label1();
        this->label1 = new Label1();
        this->label2 = new Label1();
        this->label3 = new Label1();
        this->label4 = new Label1();
        this->label5 = new Label1();
        this->label6 = new Label1();

        this->label0->set_Bounds(Rectangle(10,  10, 60, 180));
        this->label1->set_Bounds(Rectangle(70,  10, 30, 180));
        this->label2->set_Bounds(Rectangle(100, 10, 45, 180));
        this->label3->set_Bounds(Rectangle(145, 10, 60, 180));
        this->label4->set_Bounds(Rectangle(205, 10, 60, 180));
        this->label5->set_Bounds(Rectangle(265, 10, 60, 180));
        this->label6->set_Bounds(Rectangle(325, 10, 95, 180));

        this->background = Color::FromArgb(168, 189, 128);
        this->set_Location(Point(150, 50));
        this->set_ClientSize(Size(430, 200));
        this->set_Text("こんなスレッドが動いているらしい");

        syscall_set_ps_dump();
        PsInfo info;

        sprintf(format, "tid\n----\n");
        strcat(this->message1, format);
        sprintf(format, "state\n-------\n");
        strcat(this->message2, format);
        sprintf(format, "eip\n---------\n");
        strcat(this->message3, format);
        sprintf(format, "esp\n---------\n");
        strcat(this->message4, format);
        sprintf(format, "cr3\n---------\n");
        strcat(this->message5, format);
        sprintf(format, "name\n---------------\n");
        strcat(this->message6, format);

        while (syscall_read_ps_dump(&info) == 0)
        {
            sprintf(format, "%04d\n", info.tid);
            strcat(this->message1, format);
            sprintf(format, "%s\n", info.state ? "running" : "waiting");
            strcat(this->message2, format);
            sprintf(format, "%08x\n", info.eip);
            strcat(this->message3, format);
            sprintf(format, "%08x\n", info.esp);
            strcat(this->message4, format);
            sprintf(format, "%08x\n", info.cr3);
            strcat(this->message5, format);
            sprintf(format, "%s\n", info.name);
            strcat(this->message6, format);
        }

        this->label0->set_Text(
            "   ｽﾚｯﾄﾞ         \n"
            "------------     \n"
            "                 \n"
            " 　　　∧ ∧　   \n"
            "　　　(ﾟーﾟ*)    \n"
            "　　　ﾉ つつ　   \n"
            "　〜（＿OO　     \n"
            );

        this->label1->set_Text(this->message1);
        this->label2->set_Text(this->message2);
        this->label3->set_Text(this->message3);
        this->label4->set_Text(this->message4);
        this->label5->set_Text(this->message5);
        this->label6->set_Text(this->message6);

        this->get_Controls()->Add(this->label0.get());
        this->get_Controls()->Add(this->label1.get());
        this->get_Controls()->Add(this->label2.get());
        this->get_Controls()->Add(this->label3.get());
        this->get_Controls()->Add(this->label4.get());
        this->get_Controls()->Add(this->label5.get());
        this->get_Controls()->Add(this->label6.get());
    }

public:
    static void Main(Array<String> args)
    {
        Application::Run(new Form1());
    }
};

SET_MAIN_CLASS(Form1)
