#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Form.h>

#define SCREEN_W 256
#define SCREEN_H 256

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

class Vertex
{
public:
    double x;
    double y;
    double z;

public:
    Vertex(double x, double y, double z) : x(x), y(y), z(z)
    {
    }

    virtual ~Vertex() {}
};

class Face
{
public:
    Face(Vertex* v0, Vertex* v1, Vertex* v2)
    {
        this->vertex[0] = v0;
        this->vertex[1] = v1;
        this->vertex[2] = v2;
    }

    virtual ~Face() {}

public:
    Vertex* vertex[3];
};

class Canvas : public Control
{
public:
    Canvas()
    {
        this->set_BackColor(Color::get_Red());
    }

    virtual ~Canvas() {}

public:
    void OnPaint()
    {
        _P<Graphics> g = this->CreateGraphics();

        g->DrawLine(Color::get_Black(), 5, 5, 10, 10);

        g->Dispose();
    }

};


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
