#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Form.h>
#include <monalibc/math.h>
#include <monalibc/ctype.h>
#include <gui/System/Mona/Forms/Timer.h>
#include <monapi/CString.h>

#define SCREEN_W 256
#define SCREEN_H 256
#define CENTER_X (SCREEN_W / 2)
#define CENTER_Y (SCREEN_H / 2)

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

double atof(const char *s)
{
    double a = 0.0;
    int e = 0;
    int c;
    while ((c = *s++) != '\0' && isdigit(c)) {
        a = a*10.0 + (c - '0');
    }
    if (c == '.') {
        while ((c = *s++) != '\0' && isdigit(c)) {
            a = a*10.0 + (c - '0');
            e = e-1;
        }
    }
    if (c == 'e' || c == 'E') {
        int sign = 1;
        int i = 0;
        c = *s++;
        if (c == '+')
            c = *s++;
        else if (c == '-') {
            c = *s++;
            sign = -1;
        }
        while (isdigit(c)) {
            i = i*10 + (c - '0');
            c = *s++;
        }
        e += i*sign;
    }
    while (e > 0) {
        a *= 10.0;
        e--;
    }
    while (e < 0) {
        a *= 0.1;
        e++;
    }
    return a;
}

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
    Face(Vertex* v0, Vertex* v1, Vertex* v2) : nx(0), ny(0), nz(0)
    {
        this->vertex[0] = v0;
        this->vertex[1] = v1;
        this->vertex[2] = v2;
    }

    virtual ~Face() {}

    double GetDepth()
    {
        return (vertex[0]->z + vertex[1]->z + vertex[2]->z);
    }

public:
    Vertex* vertex[3];
    double nx;
    double ny;
    double nz;
};

class Canvas : public Control
{
public:
    Canvas()
    {
//        this->CreateModel();

        char* file = ReadData("/KUMA.OBJ");
        if (file == NULL)
        {
            printf("File Read Error\n");
            return;
        }

        printf("Create Model\n");
        this->CreateModel(file);
    }

    virtual ~Canvas()
    {
        this->DeleteModel();
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

    virtual char* ReadData(const char* path)
    {
        int result;
        int fileSize;
        byte* buf;

        MonAPI::FileInputStream fis(path);

        result = fis.open();

        if (result != 0) return NULL;

        fileSize = fis.getFileSize();
        buf = new byte[fileSize];

        if (buf == NULL)
        {
            fis.close();
            return NULL;
        }

        if (fis.read(buf, fileSize))
        {
            fis.close();
            return NULL;
        }

        fis.close();

        return (char*)buf;
    }

    virtual void CreateModel(MonAPI::CString content)
    {
        _A<MonAPI::CString> lines = content.split("\n");

        HList<Vertex*> vlist;
        HList<Face*> flist;

        FOREACH(MonAPI::CString, line, lines)
        {
            if (line.startsWith("v"))
            {
                _A<MonAPI::CString> values = line.split(" ");

                if (values.get_Length() == 4)
                {

                    Vertex* v = new Vertex(atof(values[1]), atof(values[2]), atof(values[3]));

                    vlist.add(v);
                }
            }
            else if (line.startsWith("f"))
            {
                _A<MonAPI::CString> values = line.split(" ");

                if (values.get_Length() == 4)
                {

                    int i1 = atoi(values[1]);
                    int i2 = atoi(values[2]);
                    int i3 = atoi(values[3]);
                    int max = vlist.size();

                    if (i1 >= max || i2 >= max || i3 >= max) continue;

                    Face* f = new Face(vlist[atoi(values[1])], vlist[atoi(values[2])], vlist[atoi(values[3])]);

                    flist.add(f);
                }
            }
        }

        this->vertexNum = vlist.size();
        this->faceNum   = flist.size();
        this->sense     = 0.01;

        this->prevX = CENTER_X;
        this->prevY = CENTER_Y;

        this->vertex = new Vertex*[this->vertexNum];
        this->face = new Face*[this->faceNum];

        for (int i = 0; i < this->vertexNum; i++)
        {
            vertex[i] = vlist[i];
        }

        for (int i = 0; i < this->faceNum; i++)
        {
            face[i] = flist[i];
        }

        this->scale = 10;
    }

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
        int maxX, midX, minX;
        double a1, a2, a3;

        GetRelativePostion(face, &maxX, &midX, &minX);
        Vertex* v1 = face->vertex[minX];
        Vertex* v2 = face->vertex[midX];
        Vertex* v3 = face->vertex[maxX];

        double x1 = v1->x * scale;
        double y1 = v1->y * scale;
        double x2 = v2->x * scale;
        double y2 = v2->y * scale;
        double x3 = v3->x * scale;
        double y3 = v3->y * scale;

        double dx1 = x2 - x1;
        double dx2 = x3 - x1;
        double dx3 = x3 - x2;

        if (dx1 != 0.0f && dx2 != 0.0f)
        {
            a1 = (y2 - y1) / dx1;
            a2 = (y3 - y1) / dx2;

            for (int x = (int)x1; x < (int)x2; x++)
            {
                int yy1 = (int)(a1 * (x - x1) + y1);
                int yy2 = (int)(a2 * (x - x1) + y1);

                g->DrawLine(color, x + CENTER_X, yy1 + CENTER_Y, x + CENTER_X, yy2 + CENTER_Y);
            }
        }

        if (dx3 != 0.0f && dx2 != 0.0f)
        {
            a2 = (y3 - y1) / dx2;
            a3 = (y3 - y2) / dx3;

            for (int x = (int)x2; x <= (int)x3; x++)
            {
                int yy1 = (int)(a2 * (x - x3) + y3);
                int yy2 = (int)(a3 * (x - x3) + y3);

                g->DrawLine(color, x + CENTER_X, yy1 + CENTER_Y, x + CENTER_X, yy2 + CENTER_Y);
            }
        }
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

    void DrawModel(_P<Graphics> graphics)
    {
        char debug[128];
        int    px[3];
        int    py[3];
        int    count = 0;
        int*    tmp = new int[faceNum];

        double* tmp_depth= new double[faceNum];
        double a1,a2,a3,b1,b2,b3;
//              sprintf(debug, "v:%s:%d", __FILE__, __LINE__);
//              syscall_print(debug);
        for(int i = 0; i < faceNum; i++)
        {
//              sprintf(debug, "v:%s:%d", __FILE__, __LINE__);
//              syscall_print(debug);
            a1 = face[i]->vertex[1]->x - face[i]->vertex[0]->x;
            a2 = face[i]->vertex[1]->y - face[i]->vertex[0]->y;
            a3 = face[i]->vertex[1]->z - face[i]->vertex[0]->z;
            b1 = face[i]->vertex[2]->x - face[i]->vertex[1]->x;
            b2 = face[i]->vertex[2]->y - face[i]->vertex[1]->y;
            b3 = face[i]->vertex[2]->z - face[i]->vertex[1]->z;
//              sprintf(debug, "v:%s:%d", __FILE__, __LINE__);
//              syscall_print(debug);
            face[i]->nx = a2 * b3 - a3 * b2;
            face[i]->ny = a3 * b1 - a1 * b3;
            face[i]->nz = a1 * b2 - a2 * b1;

            if( face[i]->nz < 0)
            {
                tmp[count] = i;
                tmp_depth[count] = face[i]->GetDepth();
                count++;
            }
            //  sprintf(debug, "v:%s:%d", __FILE__, __LINE__);
//              syscall_print(debug);
        }
//              sprintf(debug, "v:%s:%d", __FILE__, __LINE__);
//              syscall_print(debug);
        double t;
        int ti;
        int lim = count-1;

        do{
            int m = 0;
            for(int n = 0;n <= lim-1; n++)
            {
                if(tmp_depth[n] < tmp_depth[n+1])
                {
                    t = tmp_depth[n];
                    tmp_depth[n] = tmp_depth[n+1];
                    tmp_depth[n+1] = t;
                    ti = tmp[n];
                    tmp[n] = tmp[n+1];
                    tmp[n+1] = ti;
                    m = n;
                }
            }
            lim = m;
        }while(lim!=0);
//              sprintf(debug, "v:%s:%d", __FILE__, __LINE__);
//              syscall_print(debug);

        for(int m=0;m<count;m++)
        {
            int i = tmp[m];


            double l = sqrt(face[i]->nx * face[i]->nx + face[i]->ny * face[i]->ny + face[i]->nz * face[i]->nz);

            double rate = face[i]->nz / l;
            double r = 180;
            double g = 180;
            double b = 180;

            r = r + 70 * rate;
            g = g + 70 * rate;
            b = b + 70 * rate;

            Color color = Color::FromArgb((int)r, (int)g, (int)b);

            for(int j=0; j < 3; j++)
            {
                px[j]=(int)( face[i]->vertex[j]->x * scale + CENTER_X);
                py[j]=(int)(-face[i]->vertex[j]->y * scale + CENTER_Y);
            }

            int x0 = (int)( face[i]->vertex[0]->x * scale + CENTER_X);
            int x1 = (int)( face[i]->vertex[1]->x * scale + CENTER_X);
            int x2 = (int)( face[i]->vertex[2]->x * scale + CENTER_X);
            int y0 = (int)(face[i]->vertex[0]->y * scale + CENTER_Y);
            int y1 = (int)(face[i]->vertex[1]->y * scale + CENTER_Y);
            int y2 = (int)(face[i]->vertex[2]->y * scale + CENTER_Y);
            FillFace(graphics, face[i], color);
            graphics->DrawLine(color, x0, y0, x1, y1);
            graphics->DrawLine(color, x1, y1, x2, y2);
            graphics->DrawLine(color, x2, y2, x0, y0);
        }
//              sprintf(debug, "v:%s:%d", __FILE__, __LINE__);
//              syscall_print(debug);

        delete[] tmp;
        delete[] tmp_depth;
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
