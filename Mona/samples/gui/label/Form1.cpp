// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

// This file's encoding is UTF-8.

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

	Form1()
	{
		this->InitializeComponent();
	}

private:
	void InitializeComponent()
	{
        this->label1 = new Label();

        this->set_Location(Point(160, 64));
        this->set_ClientSize(Size(348, 148));
        this->set_Text("らべる");

        this->label1->set_Bounds(Rectangle(8, 8, 331, 132));
        this->label1->set_Text(
            "　★★　(´∇｀)ﾋｹﾞﾎﾟｿ作のMonaは最高のOSを目指します！　★★\n"
            "\n"
            " ∧＿∧　　／￣￣￣￣￣\n"
            "（　´∀｀）＜　オマエモナー\n"
            "（　　　　） 　＼＿＿＿＿＿\n"
            "｜ ｜　|\n"
            "（_＿）＿）\n"
            "\n"
            "　　　　　　　　　　　　　　　∧＿∧\n"
            "　　　　　　　　　　　　　　 （´∀｀　）\n"
            "''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''｀｀｀｀｀｀''''''''''''''''''");
        this->get_Controls()->Add(this->label1.get());
	}

public:
    static void Main(_A<String> args)
    {
        Application::Run(new Form1());
    }
};

SET_MAIN_CLASS(Form1)
