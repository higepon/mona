#include <monapi.h>
#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Form.h>
#include <gui/System/Mona/Forms/Label.h>

/* higepon */

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

class Form1 : public Form
{
public:
    _P<Label> label0, label1, label2, label3, label4, label5, label6;

    Form1()
    {
        this->InitializeComponent();
        this->SetLabels();
    }

private:
    char format[256];

	void SetLabels()
	{
        syscall_set_ps_dump();
        PsInfo info;

        String message1 = "tid\n----\n";
        String message2 = "state\n-------\n";
        String message3 = "eip\n---------\n";
        String message4 = "esp\n---------\n";
        String message5 = "cr3\n---------\n";
        String message6 = "name\n---------------\n";

        while (syscall_read_ps_dump(&info) == 0)
        {
            sprintf(format, "%04d\n", info.tid);
            message1 += format;
            message2 += info.state ? "running\n" : "waiting\n";
            sprintf(format, "%08x\n", info.eip);
            message3 += format;
            sprintf(format, "%08x\n", info.esp);
            message4 += format;
            sprintf(format, "%08x\n", info.cr3);
            message5 += format;
            sprintf(format, "%s\n", info.name);
            message6 += format;
        }

        this->label1->set_Text(message1);
        this->label2->set_Text(message2);
        this->label3->set_Text(message3);
        this->label4->set_Text(message4);
        this->label5->set_Text(message5);
        this->label6->set_Text(message6);
	}
	
    void InitializeComponent()
    {
        this->label0 = new Label();
        this->label1 = new Label();
        this->label2 = new Label();
        this->label3 = new Label();
        this->label4 = new Label();
        this->label5 = new Label();
        this->label6 = new Label();

        this->label0->set_Bounds(Rectangle(10,  10, 60, 180));
        this->label1->set_Bounds(Rectangle(70,  10, 30, 180));
        this->label2->set_Bounds(Rectangle(100, 10, 45, 180));
        this->label3->set_Bounds(Rectangle(145, 10, 60, 180));
        this->label4->set_Bounds(Rectangle(205, 10, 60, 180));
        this->label5->set_Bounds(Rectangle(265, 10, 60, 180));
        this->label6->set_Bounds(Rectangle(325, 10, 95, 180));

    	this->set_ForeColor(Color::FromArgb(100, 112, 76));
    	this->set_BackColor(Color::FromArgb(168, 189, 128));
        this->set_Location(Point(150, 50));
        this->set_ClientSize(Size(430, 200));
        this->set_Text("こんなスレッドが動いているらしい");

        this->label0->set_Text(
            "   ｽﾚｯﾄﾞ         \n"
            "------------     \n"
            "                 \n"
            " 　　　∧ ∧　   \n"
            "　　　(ﾟーﾟ*)    \n"
            "　　　ﾉ つつ　   \n"
            "　〜（＿OO　     \n"
            );

        this->get_Controls()->Add(this->label0.get());
        this->get_Controls()->Add(this->label1.get());
        this->get_Controls()->Add(this->label2.get());
        this->get_Controls()->Add(this->label3.get());
        this->get_Controls()->Add(this->label4.get());
        this->get_Controls()->Add(this->label5.get());
        this->get_Controls()->Add(this->label6.get());
    }

public:
    static void Main(_A<String> args)
    {
        Application::Run(new Form1());
    }
};

SET_MAIN_CLASS(Form1)
