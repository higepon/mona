// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Form.h>

#define SCREEN_W 256
#define SCREEN_H 256

extern int cube_init(int,int);
extern int cube_end( void );
extern int cube_draw(unsigned long *);

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

class BgBox : public Control
{
public:
    BgBox()
    {
        ::cube_init(SCREEN_W,SCREEN_H);
        this->set_Size(Size(SCREEN_W, SCREEN_H));
    }

    virtual ~BgBox() {
        ::cube_end();
    }

    void Next()
    {
        this->OnPaint();
        this->RefreshInternal();
    }

protected:
    virtual void OnPaint(){
        ::cube_draw((unsigned long *)this->buffer->get());
    }
};

class Form1 : public Form
{
public:
    _P<BgBox> bgbox;

    Form1()
    {
        this->InitializeComponent();
    }

private:
    void InitializeComponent()
    {
        this->bgbox = new BgBox();

        this->set_Location(Point(128, 128));
        this->set_ClientSize(Size(SCREEN_W, SCREEN_H));
        this->set_Text(":: cube ::");

        this->get_Controls()->Add(this->bgbox.get());
    }

public:
    static void Main(_A<String> args)
    {
        _P<Form1> f = new Form1();
        for (f->Show(); f->get_Visible(); Application::DoEvents())
        {
            f->bgbox->Next();
        }
    }
};

SET_MAIN_CLASS(Form1)
