#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Form.h>
#include <monalibc/math.h>
#include <gui/System/Mona/Forms/Timer.h>

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
        this->CreateModel();
    }

    virtual ~Canvas()
    {
        this->DeleteModel();
    }

protected:
    virtual void OnMouseMove(_P<MouseEventArgs> e)
    {
        if (this->drag)
        {
            int dx = e->X - this->prevX;
            int dy = e->Y - this->prevY;

            double phi   =  dx * this->sense;
            double theta =  dy * this->sense;

            this->Rotate(phi,theta);
            this->Refresh();

            this->prevX = e->X;
            this->prevY = e->Y;
        }
        Control::OnMouseMove(e);
    }

    virtual void OnMouseDown(_P<MouseEventArgs> e)
    {
        this->drag = true;
        prevX = e->X;
        prevY = e->Y;
        Control::OnMouseDown(e);
    }

    virtual void OnMouseUp(_P<MouseEventArgs> e)
    {
        this->drag = false;
        Control::OnMouseUp(e);
    }

    virtual void OnMouseLeave(_P<EventArgs> e)
    {
        this->drag = false;
        Control::OnMouseLeave(e);
    }

    virtual void FillFace(_P<Graphics> g, Face* face, Color color)
    {
        int maxX;
        int midX;
        int minX;
        Vertex* v1;
        Vertex* v2;
        Vertex* v3;
	double x1, x2, x3;
	double y1, y2, y3;

        GetRelativePostion(face, &maxX, &midX, &minX);
        v1 = face->vertex[minX];
        v2 = face->vertex[midX];
        v3 = face->vertex[maxX];

	x1 = v1->x * scale;
	y1 = v1->y * scale;
	x2 = v2->x * scale;
	y2 = v2->y * scale;
	x3 = v3->x * scale;
	y3 = v3->y * scale;

	double dx1 = x2 - x1;
	double dx2 = x3 - x1;
	double dx3 = x3 - x2;

	if (dx1 == 0.0f || dx2 == 0.0f) return;

	double a1 = (y2 - y1) / dx1;
	double a2 = (y3 - y1) / dx2;

	for (int x = (int)x1; x < (int)x2; x++)
	{
	    int yy1 = (int)(a1 * (x - x1) + y1);
	    int yy2 = (int)(a2 * (x - x1) + y1);
	    int s = yy1 > yy2 ? -1 : 1;

            g->DrawLine(color, x + CENTER_X, yy1 + CENTER_Y + s, x + CENTER_X, yy2 + CENTER_Y - s);
	}

	if (dx3 == 0.0f) return;

	double a3 = (y3 - y2) / dx3;

	for (int x = (int)x2; x < (int)x3; x++)
	{
	    int yy1 = (int)(a2 * (x - x3) + y3);
	    int yy2 = (int)(a3 * (x - x3) + y3);
	    int s = yy1 > yy2 ? -1 : 1;

            g->DrawLine(color, x + CENTER_X, yy1 + CENTER_Y + s, x + CENTER_X, yy2 + CENTER_Y - s);
	}




// 	if ((v2->x - v1->x) == 0 ||  (v3->x - v1->x) == 0) return;

//         double a1 = (v2->y - v1->y) / (v2->x - v1->x);
//         double a2 = (v3->y - v1->y) / (v3->x - v1->x);

//         for (int x = (int)(v1->x * scale); x < (int)(v2->x * scale); x++)
//         {
//             int y1 = (int)(a1 * (x - v1->x) * scale + v1->y * scale);
//             int y2 = (int)(a2 * (x - v1->x) * scale + v1->y * scale);

// 	char buf[512];
// 	sprintf(buf, "(%d,%d,%d,%d)<%d, %d>", x, y1, x, y2, (int)(a1 * (x - v1->x) * scale), (int)(a2 * (x - v1->x) * scale));
// 	sprintf(buf, "[%1.99f, %1.99f]", v2->x, v1->x);
// 	syscall_print(buf);

//            g->DrawLine(color, x + CENTER_X, y1 + CENTER_Y, x + CENTER_X, y2 + CENTER_Y);
	//      }
    }

    virtual void GetRelativePostion(Face* face, int* maxX, int* midX, int* minX)
    {
        double x[3];
        x[0] = face->vertex[0]->x;
        x[1] = face->vertex[1]->x;
        x[2] = face->vertex[2]->x;

        if (x[0] >= x[1] && x[0] >= x[2])
        {
            *maxX = 0;

            if (x[1] >= x[2])
            {
                *midX = 1;
                *minX = 2;
            }
            else
            {
                *midX = 2;
                *minX = 1;
            }
        }
        else if (x[0] >= x[1] && x[0] < x[2])
        {
            *maxX = 2;
            *midX = 0;
            *minX = 1;

        }
        else if (x[0] < x[1] && x[0] >= x[2])
        {
            *maxX = 1;
            *midX = 0;
            *minX = 2;
        }
        else
       {
           *minX = 0;

           if (x[1] >= x[2])
           {
               *maxX = 1;
               *midX = 2;
           }
           else
           {
               *maxX = 2;
               *midX = 1;
           }
       }
    }

public:
    void OnPaint()
    {
        _P<Graphics> g = this->CreateGraphics();

        g->FillRectangle(Color::get_White(), 0, 0, SCREEN_W, SCREEN_H);
        this->DrawModel(g);

        g->Dispose();
    }

    void Rotate(double phi, double theta)
    {
        double tx, ty, tz;
        double tmpSin = sin(theta);
        double tmpCos = cos(theta);

        for (int i = 0; i < vertexNum; i++)
        {
            ty = vertex[i]->y * tmpCos + vertex[i]->z * tmpSin;
            tz = -vertex[i]->y * tmpSin + vertex[i]->z * tmpCos;
            vertex[i]->y = ty;
            vertex[i]->z = tz;

        }

        tmpSin = sin(phi);
        tmpCos = cos(phi);

        for (int i = 0; i < vertexNum; i++)
        {
            tx = vertex[i]->x * tmpCos - vertex[i]->z * tmpSin;
            tz = vertex[i]->x * tmpSin + vertex[i]->z * tmpCos;
            vertex[i]->x = tx;
            vertex[i]->z = tz;
        }
    }

protected:

    void CreateModel()
    {
        this->vertexNum = 6;
        this->faceNum   = 8;
        this->sense     = 0.01;

        this->prevX = CENTER_X;
        this->prevY = CENTER_Y;

        this->vertex = new Vertex*[this->vertexNum];
        this->face = new Face*[this->faceNum];

        vertex[0] = new Vertex(-1, 0, 0);
        vertex[1] = new Vertex( 0, 0, 1);
        vertex[2] = new Vertex( 0, 1, 0);
        vertex[3] = new Vertex( 1, 0, 0);
        vertex[4] = new Vertex( 0,-1, 0);
        vertex[5] = new Vertex( 0, 0,-1);

        face[0] = new Face( vertex[1], vertex[4], vertex[2]);
        face[1] = new Face( vertex[1], vertex[0], vertex[4]);
        face[2] = new Face( vertex[1], vertex[2], vertex[0]);
        face[3] = new Face( vertex[3], vertex[2], vertex[4]);
        face[4] = new Face( vertex[0], vertex[5], vertex[4]);
        face[5] = new Face( vertex[4], vertex[5], vertex[3]);
        face[6] = new Face( vertex[3], vertex[5], vertex[2]);
        face[7] = new Face( vertex[2], vertex[5], vertex[0]);

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
            FillFace(g, face[i], Color::get_Yellow());
        }
    }

protected:
    bool drag;
    int prevX;
    int prevY;
    int vertexNum;
    int faceNum;
    double scale;
    double sense;
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
