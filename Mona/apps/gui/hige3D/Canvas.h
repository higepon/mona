#ifndef __CANVAS_H__
#define __CANVAS_H__

#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Form.h>
#include <monalibc/math.h>
#include <monalibc/ctype.h>
#include <gui/System/Mona/Forms/Timer.h>
#include <monapi/CString.h>

#include "Face.h"
#include "notmine.h"

#define SCREEN_W 256
#define SCREEN_H 256
#define CENTER_X (SCREEN_W / 2)
#define CENTER_Y (SCREEN_H / 2)

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

class Canvas : public Control
{
public:
    Canvas()
    {
/*         char* file = ReadData("/KUMA.OBJ"); */
/*         if (file == NULL) */
/*         { */
/*             printf("File Read Error\n"); */
/*             return; */
/*         } */

        v0 = new Vertex(0, 0, 0);
        v1 = new Vertex(0, 0, 0);
        v2 = new Vertex(0, 0, 0);
        tempFace = new Face(v0, v1, v2);

//        this->CreateModel(file);
        this->CreateModel();
        this->Rotate(0, 3.14 / 4);
    }

    virtual ~Canvas()
    {
        this->DeleteModel();
        delete v0;
        delete v1;
        delete v2;
        delete tempFace;
    }

public:
    void OnPaint()
    {
        _P<Graphics> g = this->CreateGraphics();

        g->FillRectangle(Color::get_White(), 0, 0, SCREEN_W, SCREEN_H);
        this->DrawModel(g);

        g->Dispose();
    }

protected:

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

    /*----------------------------------------------------------------------
        Event Handler
    ----------------------------------------------------------------------*/
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

    /*----------------------------------------------------------------------
        Draw Utility
    ----------------------------------------------------------------------*/
    virtual void FillFace(_P<Graphics> g, Face* face, Color color)
    {
        int maxX, midX, minX;
        double a1, a2, a3;

        GetRelativePostion(face, &maxX, &midX, &minX);
        Vertex* v1 = face->vertex[minX];
        Vertex* v2 = face->vertex[midX];
        Vertex* v3 = face->vertex[maxX];

        double x1 = v1->x;
        double y1 = v1->y;
        double x2 = v2->x;
        double y2 = v2->y;
        double x3 = v3->x;
        double y3 = v3->y;

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

                g->DrawLine(color, x, yy1, x, yy2);
            }
        }

        if (dx3 != 0.0f && dx2 != 0.0f)
        {
            a2 = (y3 - y1) / dx2;
            a3 = (y3 - y2) / dx3;

            for (int x = (int)x2; x < (int)x3; x++)
            {
                int yy1 = (int)(a2 * (x - x3) + y3);
                int yy2 = (int)(a3 * (x - x3) + y3);

                g->DrawLine(color, x, yy1, x, yy2);
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

    /*----------------------------------------------------------------------
        Read Data
    ----------------------------------------------------------------------*/
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

        this->scale = 15;
    }

    void DrawModel(_P<Graphics> graphics)
    {
        int    px[3];
        int    py[3];
        int    count = 0;
        int*    tmp = new int[faceNum];

        double* tmp_depth= new double[faceNum];
        double a1,a2,a3,b1,b2,b3;

        for(int i = 0; i < faceNum; i++)
        {
            a1 = face[i]->vertex[1]->x - face[i]->vertex[0]->x;
            a2 = face[i]->vertex[1]->y - face[i]->vertex[0]->y;
            a3 = face[i]->vertex[1]->z - face[i]->vertex[0]->z;
            b1 = face[i]->vertex[2]->x - face[i]->vertex[1]->x;
            b2 = face[i]->vertex[2]->y - face[i]->vertex[1]->y;
            b3 = face[i]->vertex[2]->z - face[i]->vertex[1]->z;
            face[i]->nx = a2 * b3 - a3 * b2;
            face[i]->ny = a3 * b1 - a1 * b3;
            face[i]->nz = a1 * b2 - a2 * b1;

            if( face[i]->nz < 0)
            {
                tmp[count] = i;
                tmp_depth[count] = face[i]->GetDepth();
                count++;
            }
        }

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

        for(int m=0;m<count;m++)
        {
            int i = tmp[m];


            double l = sqrt(face[i]->nx * face[i]->nx + face[i]->ny * face[i]->ny + face[i]->nz * face[i]->nz);

            int B = (int)( 75 -180 * face[i]->nz / l);
            if (B < 0) B = 0;
            if (B > 255) B = 255;

            double r, g, b;

            HSBtoRGB((float)0.4, (float)0.25, (float)B / 255, &r, &g, &b);
            Color color = Color::FromArgb((int)(r * 255), (int)(g * 255), (int)(b * 255));

            for(int j=0; j < 3; j++)
            {
                px[j]=(int)( face[i]->vertex[j]->x * scale + CENTER_X);
                py[j]=(int)(-face[i]->vertex[j]->y * scale + CENTER_Y);
            }

            tempFace->vertex[0]->x = px[0];
            tempFace->vertex[0]->y = py[0];
            tempFace->vertex[1]->x = px[1];
            tempFace->vertex[1]->y = py[1];
            tempFace->vertex[2]->x = px[2];
            tempFace->vertex[2]->y = py[2];

/*             int x0 = (int)( face[i]->vertex[0]->x * scale + CENTER_X); */
/*             int x1 = (int)( face[i]->vertex[1]->x * scale + CENTER_X); */
/*             int x2 = (int)( face[i]->vertex[2]->x * sc]ale + CENTER_X); */
/*             int y0 = (int)(face[i]->vertex[0]->y * scale + CENTER_Y); */
/*             int y1 = (int)(face[i]->vertex[1]->y * scale + CENTER_Y); */
/*             int y2 = (int)(face[i]->vertex[2]->y * scale + CENTER_Y); */

            FillFace(graphics, tempFace, color);

//            graphics->DrawLine(color, x0, y0, x1, y1);
//            graphics->DrawLine(color, x1, y1, x2, y2);
//            graphics->DrawLine(color, x2, y2, x0, y0);
        }

        delete[] tmp;
        delete[] tmp_depth;
    }

    void CreateModel();

protected:
    bool drag;
    int prevX;
    int prevY;
    int vertexNum;
    int faceNum;
    double scale;
    double sense;
    Vertex** vertex;
    Vertex* v0;
    Vertex* v1;
    Vertex* v2;
    Face* tempFace;
    Face** face;
};

#endif
