#ifndef __FACE_H__
#define __FACE_H__

#include "Vertex.h"

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

#endif
