// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.
// Higepon 2004

// This file's encoding is UTF-8.

#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Form.h>
#include <gui/System/Mona/Forms/Label.h>
#include <gui/System/Mona/Forms/Timer.h>
#include <gui/System/Mona/Forms/ControlPaint.h>
#include <gui/System/Drawing/Font.h>
#include <monalibc/math.h>

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

class Form1 : public Form
{
private:
  _P<Timer> timer1;
  _P<Font> font;
  int count, oldsec, nullpo, xcenter, ycenter;
  MonAPI::Date date;

public:
  Form1()
  {
    this->InitializeComponent();
  }

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
    this->set_Location(Point(50, 50));
    this->set_ClientSize(Size(150, 50));

    nullpo = 0;
    oldsec = -1;
    ycenter = 95 - 18;
    xcenter = 75 - 2;

    this->set_Location(Point(50, 50));
    this->set_ClientSize(Size(150, 150));
    this->set_Text("ぬるぽ時計");

    // Tino: for transparency
    this->set_BackColor(Color::get_Empty());
    this->set_Opacity(0.8);

    this->timer1 = new Timer();
    this->timer1->set_Interval(500);
    this->timer1->add_Tick(new EventHandler<Form1>(this, &Form1::PaintClock));
  }

  int GetX(double angle, double length){
    return (int) (sin(angle) * length + xcenter);
  }

  int GetY(double angle, double length){
    return (int) (-1.0 * cos(angle) * length) + ycenter ;
  }


  void PaintClock(_P<Object> sender, _P<EventArgs> e)
  {

    this->Refresh();

  }

  virtual void OnPaint()
  {

    Form::OnPaint();

    _P<Graphics> g = this->CreateGraphics();
    double PI = 3.14159265, dh, dm;
    int i = 1;

    // Clear
#if 1  // Tino
    g->FillEllipse(ControlPaint::get_Light(), 3, 7, 140, 140);
#else
    int w = this->get_Width(), h = this->get_Height();
    g->FillRectangle(this->get_BackColor(), 0, 18, w, h - 18);

    // Border
    ControlPaint::DrawEngraved(g, 0, 18, w, h - 18);
#endif

    g->DrawEllipse(Color::get_Black(), 3, 7, 140, 140);

    // 目盛の描画
    for(i = 0;i < 360;i += 6){
      if(i % 30 == 0)
        g->DrawLine(Color::get_Black(),GetX( (double)(PI / 180 * i), 60), GetY((double)(PI / 180 * i), 61), GetX( (double)(PI / 180 * i), 65), GetY((double)(PI / 180 * i), 65));
      else
        g->DrawLine(Color::get_Black(),GetX( (double)(PI / 180 * i), 64), GetY((double)(PI / 180 * i), 64), GetX( (double)(PI / 180 * i), 65), GetY((double)(PI / 180 * i), 65));
    }

    // Date
    this->date.refresh();

    // 秒針の描画
    g->DrawLine(Color::get_Black(), xcenter, ycenter, GetX( (double)(PI / 180 * date.sec() * 6), 60), GetY((double)(PI / 180 * date.sec() * 6), 60));

    dh = (double)(date.hour() % 12 * 30) + ((double)(date.min() * 60.0 + date.sec()) * (30.0 / 3600.0));
    dm = (double)(date.min() * 6 + date.sec() * 0.1);

    g->DrawLine(Color::get_Black(), xcenter, ycenter, GetX((PI / 180.0 * dm), 55), GetY((double)(PI / 180.0 * dm), 55));
    g->DrawLine(Color::get_Black(), xcenter, ycenter, GetX((PI / 180.0 * dh), 35), GetY((double)(PI / 180.0 * dh), 35));
    g->FillEllipse(Color::get_Black(), xcenter - 2, ycenter - 2, 5, 5);

    //nullpoの描画
    if((oldsec != date.sec() & nullpo == 0) || (oldsec == date.sec() & nullpo == 1)){
      g->DrawString("　∧＿∧ 　　\n"
                    "　（　´∀｀）＜　ぬるぽ ", Control::get_DefaultFont(), Color::get_Blue(), 18, 52);
      nullpo = 1;
      oldsec = date.sec();
    }else{
      g->DrawString("　 （　・∀・）　　　|　|　ｶﾞｯ \n"
                    "　  と　　　）　 　 |　| \n"
                    "　　 Ｙ　/ノ　　　 人 \n"
                    "　　　 /　）　 　 < 　>__Λ∩\n"
                    "　 ＿/し'　／／. Ｖ｀Д´）/ \n"
                    "　（＿フ彡　　　　　 　　/\n", Control::get_DefaultFont(), Color::get_Blue(), 8, 47);
      nullpo = 0;
      oldsec = date.sec();
    }
    g->Dispose();

  }

public:
  static void Main(_A<String> args)
  {
    Application::Run(new Form1());
  }
};

SET_MAIN_CLASS(Form1)
