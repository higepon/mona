#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Form.h>

#define SCREEN_W 256
#define SCREEN_H 256
#define CENTER_X (SCREEN_W / 2)
#define CENTER_Y (SCREEN_H / 2)

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
        this->set_BackColor(Color::get_White());

        this->CreateModel();
    }

    virtual ~Canvas()
    {
        this->DeleteModel();
    }

public:
    void OnPaint()
    {
        _P<Graphics> g = this->CreateGraphics();

        this->DrawModel(g);

        g->Dispose();
    }

protected:

    void DrawModel(_P<Graphics> g)
    {
        for (int i = 0; i < faceNum; i++)
        {
            int x0 = (int)(face[i]->vertex[0]->x * this->scale + CENTER_X);
            int x1 = (int)(face[i]->vertex[1]->x * this->scale + CENTER_X);
            int x2 = (int)(face[i]->vertex[2]->x * this->scale + CENTER_X);
            int y0 = (int)(face[i]->vertex[0]->y * this->scale + CENTER_Y);
            int y1 = (int)(face[i]->vertex[1]->y * this->scale + CENTER_Y);
            int y2 = (int)(face[i]->vertex[2]->y * this->scale + CENTER_Y);
            g->DrawLine(Color::get_Blue(), x0, y0, x1, y1);
            g->DrawLine(Color::get_Blue(), x1, y1, x2, y2);
            g->DrawLine(Color::get_Blue(), x2, y2, x0, y0);

        }
    }

    void CreateModel()
    {
        this->vertexNum = 6;
        this->faceNum = 8;

        this->vertex = new Vertex*[this->vertexNum];
        this->face = new Face*[this->faceNum];

        vertex[0]=new Vertex(-1, 0, 0);
        vertex[1]=new Vertex( 0, 0, 1);
        vertex[2]=new Vertex( 0, 1, 0);
        vertex[3]=new Vertex( 1, 0, 0);
        vertex[4]=new Vertex( 0,-1, 0);
        vertex[5]=new Vertex( 0, 0,-1);

        face[0]=new Face( vertex[1], vertex[4], vertex[2]);
        face[1]=new Face( vertex[1], vertex[0], vertex[4]);
        face[2]=new Face( vertex[1], vertex[2], vertex[0]);
        face[3]=new Face( vertex[3], vertex[2], vertex[4]);
        face[4]=new Face( vertex[0], vertex[5], vertex[4]);
        face[5]=new Face( vertex[4], vertex[5], vertex[3]);
        face[6]=new Face( vertex[3], vertex[5], vertex[2]);
        face[7]=new Face( vertex[2], vertex[5], vertex[0]);

        this->scale = 100;
    }

    void DeleteModel()
    {
        for (int i = 0; i < vertexNum; i++)
        {
            delete vertex[i];
        }

        for (int i = 0; i < faceNum; i++)
        {
            delete face[i];
        }

        delete[] this->vertex;
        delete[] this->face;
    }

protected:
    int vertexNum;
    int faceNum;
    double scale;
    Vertex** vertex;
    Face** face;
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
