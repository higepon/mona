// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Form.h>

#define SCREEN_W 256
#define SCREEN_H 256

extern void noiz2bgInit();
extern void noiz2bgPaint(unsigned char *pbuf);

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

class Noiz2bgBox : public Control
{
public:
	Noiz2bgBox()
	{
		::noiz2bgInit();
		this->set_Size(Size(SCREEN_W, SCREEN_H));
	}
	
	virtual ~Noiz2bgBox() {}
	
	void Next()
	{
		this->Refresh();
	}

protected:
	virtual void OnPaint()
	{
		::noiz2bgPaint((unsigned char*)this->buffer->get());
	}
};

class Form1 : public Form
{
public:
	_P<Noiz2bgBox> noiz2bgBox;

	Form1()
	{
		this->InitializeComponent();
	}

private:
	void InitializeComponent()
	{
        this->noiz2bgBox = new Noiz2bgBox();

        this->set_Location(Point(128, 128));
        this->set_ClientSize(Size(SCREEN_W, SCREEN_H));
        this->set_Text("noiz2bg for GUI");

        this->get_Controls()->Add(this->noiz2bgBox.get());
	}

public:
    static void Main(_A<String> args)
    {
    	_P<Form1> f = new Form1();
    	for (f->Show(); f->get_Visible(); Application::DoEvents())
    	{
    		f->noiz2bgBox->Next();
    	}
    }
};

SET_MAIN_CLASS(Form1)
