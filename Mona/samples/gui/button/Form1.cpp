// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#include <gui/System/Console.h>
#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Button.h>
#include <gui/System/Mona/Forms/Form.h>

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

class Form1 : public Form
{
public:
	_P<Button> button1;
	
	Form1()
	{
		this->InitializeComponent();
	}
	
private:
	void InitializeComponent()
	{
		this->button1 = new Button();
		
		this->set_Location(Point(20, 64));
		this->set_ClientSize(Size(110, 36));
		this->set_Text("ぼたん");
		
		this->button1->set_Bounds(Rectangle(8, 8, 94, 20));
		this->button1->set_Text("ヽ(`Д´)ﾉｳﾜｧｧﾝ!!");
		this->button1->add_Click(new EventHandler<Form1>(this, &Form1::button1_Click));
		this->get_Controls()->Add(this->button1.get());
	}
	
	void button1_Click(_P<Object> sender, _P<EventArgs> e)
	{
		Console::WriteLine("Uwaaaaaaan!!");
	}
	
public:
	static void Main(_A<String> args)
	{
		Application::Run(new Form1());
	}
};

SET_MAIN_CLASS(Form1)
