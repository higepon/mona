#ifndef __VERTEX_H__
#define __VERTEX_H__

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

#endif
