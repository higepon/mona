#include "Canvas.h"

class Form1 : public Form
{
public:
    Form1()
    {
        this->InitializeComponent();
    }

private:
    _P<Canvas> canvas;

private:
    void InitializeComponent()
    {
        this->set_Location(Point(128, 128));
        this->set_ClientSize(Size(SCREEN_W, SCREEN_H));
        this->set_Text("::GHige3D");

        canvas = new Canvas();
        canvas->set_Bounds(Rectangle(Point::get_Empty(), this->get_ClientSize()));
        this->get_Controls()->Add(canvas.get());
    }

public:
    static void Main(_A<String> args)
    {
        Application::Run(new Form1());
    }
};

SET_MAIN_CLASS(Form1)
