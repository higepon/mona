// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#include <gui/System/Console.h>
#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Form.h>
#include <gui/System/Mona/Forms/Label.h>

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

class Form1 : public Form
{
public:
	_P<Bitmap> bitmap;

    Form1(String fn, _P<Bitmap> bmp)
    {
    	this->set_Text(fn);
    	this->set_ClientSize(Size(bmp->get_Width(), bmp->get_Height()));
    	this->bitmap = bmp;
    }
    
    virtual ~Form1() {}
    
    virtual void OnPaint()
    {
    	Form::OnPaint();
    	
    	_P<Graphics> g = this->CreateGraphics();
    	g->DrawImage(this->bitmap, 0, 0);
    	g->Dispose();
    }

    static void Main(Array<String> args)
    {
    	if (args.get_Length() == 0)
    	{
    		Console::WriteLine("usage: BITMAP FILENAME.BMP");
    		return;
    	}
    	
    	_P<Bitmap> bmp = new Bitmap(args[0]);
    	if (bmp->get() == NULL)
    	{
    		Console::WriteLine("Can not open file!");
    		return;
    	}
    	
        Application::Run(new Form1(args[0], bmp));
    }
};

SET_MAIN_CLASS(Form1)
