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
    Pointer<Label> label1;

    Form1()
    {
        this->InitializeComponent();

    }

private:
    void InitializeComponent()
    {
        this->background = Color::FromArgb(255, 246, 168);

        this->label1 = new Label();

        this->set_Location(Point(160, 64));
        this->set_ClientSize(Size(348, 200));
        this->set_Text("こんなスレッドが動いているらしい");

        this->label1->set_Bounds(Rectangle(8, 8, 331, 190));

        char message[1024];
        char format[256];

        message[0] = '\0';

        syscall_set_ps_dump();

        PsInfo info;

        sprintf(format, "\n|-`).｡ｏO(カーネルの中の人も大忙し)\n\n");
        strcat(message, format);
        sprintf(format, "tid      eip      esp       cr3        状態         お名前\n");
        strcat(message, format);
        sprintf(format, "----------------------------------------------------------\n");
        strcat(message, format);

        while (syscall_read_ps_dump(&info) == 0)
        {
            sprintf(format, "%04d %08x %08x %08x %s %s\n", info.tid, info.eip, info.esp, info.cr3, info.state ? "running" : "waiting", info.name);
            strcat(message, format);
        }

        this->label1->set_Text(message);
        this->get_Controls()->Add(this->label1.get());
    }

public:
    static void Main(Array<String> args)
    {
        Application::Run(new Form1());
    }
};

SET_MAIN_CLASS(Form1)
