// Encoding UTF-8N...

#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Form.h>
#include <gui/System/Mona/Forms/Label.h>
#include <gui/System/Mona/Forms/Timer.h>

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

	/* OSASKパレット */
	static int osapal_r[9] = {0, 128, 0, 128, 0, 128, 0, 128, 192};
	static int osapal_g[9] = {0, 0, 128, 128, 0, 0, 128, 128, 192};
	static int osapal_b[9] = {0, 0, 0, 0, 128, 128, 128, 128, 192};

	static int col = 0;

class Form1 : public Form
{
public:
    _P<Label> label1;
    _P<Timer> timer1;

    Form1()
    {
	this->InitializeComponent();
	this->chgcol(this, EventArgs::get_Empty());
    }

private:
    void chgcol(_P<Object> sender, _P<EventArgs> e)
    {
	label1->set_ForeColor(Color::FromArgb(osapal_r[col], osapal_g[col], osapal_b[col]));
	col++;
	if (col > 7)
		col = 0;
    }

protected:
    virtual void Create()
    {
        Form::Create();
        this->timer1->Start();
    }

    virtual void Dispose()
    {
        this->timer1->Dispose();
        Form::Dispose();
    }


private:
    void InitializeComponent()
    {
	this->label1 = new Label();

    	this->set_BackColor(Color::FromArgb(osapal_r[8], osapal_g[8], osapal_b[8]));
	/* ウィンドウの位置＜Point(Xpos, Ypos)＞ */
	this->set_Location(Point(200, 64));
	/* ウィンドウのサイズ。＜Size(Xsize, Ysize)＞ */
	this->set_ClientSize(Size(14 * 8, 4 * 16));
	this->set_Text(" M t . o r z 0 ");

	/* ＜Rectangle(Xpos, Ypos, Xsize, Ysize)＞ */
	this->label1->set_Bounds(Rectangle(20, 8, 12 * 8, 3 * 16));
	this->label1->set_Text(
	    "　　　orz\n"
	    "　　orz orz\n"
	    "　orz orz orz\n"
	    "orz orz orz orz");

	this->get_Controls()->Add(this->label1.get());

	this->timer1 = new Timer();
	this->timer1->set_Interval(500);
	this->timer1->add_Tick(new EventHandler<Form1>(this, &Form1::chgcol));
    }

public:
    static void Main(Array<String> args)
    {
        Application::Run(new Form1());
    }
};

SET_MAIN_CLASS(Form1)
