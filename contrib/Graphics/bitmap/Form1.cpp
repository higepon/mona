// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#include <gui/System/Console.h>
#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Form.h>
#include <gui/System/Mona/Forms/Label.h>
#include <stdlib.h>

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

class Form1 : public Form
{
public:
    _P<Bitmap> bitmap;

    Form1(String fn, _P<Bitmap> bmp, int x, int y)
    {
        this->set_Text(fn);
        this->set_Location(Point(x, y));
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

    static int watoi(wchar* text)
    {
        char buf[32];
        for (int i = 0; ; i++)
        {
            if (text[i] == '\0') break;
            buf[i] = text[i];
        }
        return atoi(buf);
    }

    static void Main(_A<String> args)
    {
        if (args.get_Length() == 0)
        {
            Console::WriteLine("usage: BITMAP FILENAME.BMP/JPG");
            return;
        }

        _P<Bitmap> bmp = new Bitmap(args[0]);
        if (bmp->get() == NULL)
        {
            Console::WriteLine("Can not open file!");
            return;
        }

        int x = 100;
        int y = 100;
        if (args.get_Length() >= 3)
        {
            x = watoi(args[1].get());
            y = watoi(args[2].get());
        }
        Application::Run(new Form1(args[0], bmp, x, y));
    }
};

SET_MAIN_CLASS(Form1)
