// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Form.h>

#define SCREEN_W 160
#define SCREEN_H 100

extern "C" int DrawFire();

int typeDraw = 0, FireSeed = 0x1234;
unsigned char calc, * FireScreen, * pImage;

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

class FireBox : public Control
{
private:
	_A<unsigned char> fireScreen, image;

public:
	FireBox()
	{
		this->fireScreen.Alloc(0x2300);
		::FireScreen = this->fireScreen.get();
		this->image.Alloc(SCREEN_W * SCREEN_H * 3);
		::pImage = this->image.get();
		this->set_Size(Size(SCREEN_W, SCREEN_H));
	}
	
	virtual ~FireBox() {}
	
	void Next()
	{
		if (::DrawFire() == 0) return;
		
		this->OnPaint();
		this->RefreshInternal();
	}

protected:
	virtual void OnPaint()
	{
		int len = SCREEN_W * SCREEN_H;
		unsigned char* dst = (unsigned char*)this->buffer->get();
		unsigned char* src = this->image.get();
		for (int i = 0; i < len; i++, dst += 4, src += 3)
		{
			dst[0] = src[2];
			dst[1] = src[1];
			dst[2] = src[0];
			dst[3] = 0xff;
		}
	}
};

class Form1 : public Form
{
public:
	_P<FireBox> fireBox;

	Form1()
	{
		this->InitializeComponent();
	}

private:
	void InitializeComponent()
	{
        this->fireBox = new FireBox();

        this->set_Location(Point(100, 100));
        this->set_ClientSize(Size(SCREEN_W, SCREEN_H));
        this->set_Text("FIRE EFFECT");

        this->get_Controls()->Add(this->fireBox.get());
	}

public:
    static void Main(_A<String> args)
    {
    	_P<Form1> f = new Form1();
    	for (f->Show(); f->get_Visible(); Application::DoEvents())
    	{
    		f->fireBox->Next();
    	}
    }
};

SET_MAIN_CLASS(Form1)
