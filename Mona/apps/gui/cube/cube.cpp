#include "geometry.h"
#include <monapi.h>
using namespace MonAPI;

/* ポリゴンデータ */
Vector vertex[8];
int index[24];
unsigned long mat[12];

Matrix world;
float angle = 0.0f;

/* global settings */
int width,height;
unsigned long *buf;

int cube_init(int _width,int _height){
    width = _width;
    height= _height;
    
    // setup vertexs
    vertex[0].set( -1.0f, -1.0f, -1.0f); // XYZ
    vertex[1].set(  1.0f, -1.0f, -1.0f); // xYZ
    vertex[2].set(  1.0f,  1.0f, -1.0f); // xyZ
    vertex[3].set( -1.0f,  1.0f, -1.0f); // XyZ

    vertex[4].set( -1.0f, -1.0f,  1.0f);
    vertex[5].set(  1.0f, -1.0f,  1.0f);
    vertex[6].set(  1.0f,  1.0f,  1.0f);
    vertex[7].set( -1.0f,  1.0f,  1.0f);

    index[0] = 0; index[1] = 1;  mat[0] = 0x0000FF;
    index[2] = 1; index[3] = 2;  mat[1] = 0x00FF00;
    index[4] = 2; index[5] = 3;  mat[2] = 0xFF0000;
    index[6] = 3; index[7] = 0;  mat[3] = 0xFF00FF;

    index[8]  = 4; index[9]  = 5;  mat[4] = 0xFFFF00;
    index[10] = 5; index[11] = 6;  mat[5] = 0xFFFFFF;
    index[12] = 6; index[13] = 7;  mat[6] = 0x0000CC;
    index[14] = 7; index[15] = 4;  mat[7] = 0x00CC00;

    index[16] = 0; index[17] = 4;  mat[8] = 0xCC0000;
    index[18] = 1; index[19] = 5;  mat[9] = 0xCC00CC;
    index[20] = 2; index[21] = 6;  mat[10] = 0xCCCC00;
    index[22] = 3; index[23] = 7;  mat[11] = 0xCCCCCC;
    
    return 0;
}
int cube_end(void){
    return 0;
}

float inline abs(float f){
    return (f>0.0) ? f : 0.0 - f;
}

void dot(float fx,float fy, unsigned long color ){
    int x,y;
    x = (int) fx;
    y = (int) fy;
    if(x<0 || y<0 || x>=width || y>=width)
        return;
    buf[ x + ( y*width ) ] = 0xFF000000 | color;
}

void line(float x1,float y1,float x2,float y2,unsigned long color)
{
    int dt,t;
    float x,y,r;
    dt = (int)(abs(y2-y1) + abs(x2-x1))*2; // 分割数
    for(t=0;t<dt;t++) {
        x = x1 + (x2 - x1) * t / dt;
        y = y1 + (y2 - y1) * t / dt;
        dot(x,y, color);
    }
}


int cube_draw(unsigned long *target){
    int i,k,v;
    float x[8],y[8];
    Vector temp;
    float dist = 100.0f;
    Matrix m1,m2,m3;

    angle += 0.01f;
    m1.SetXrotate(angle);
    m2.SetYrotate(angle);
    m3.SetVector( 0.0f, 0.0f, -4.0f);

    world = m1 * m2 * m3;
    
    // 透視変換とかする
    for(i=0;i<8;i++){
        temp = vertex[i] * world;
        x[i] = (width / 2.0f) + temp.x * dist / temp.z ;
        y[i] = (height/ 2.0f) + temp.y * dist / temp.z ;
    }
    
    buf = target;
    for(i=0;i<width*height;i++){
        buf[i] = 0xFFFFFFFF;
    }
    for(i=0;i<12;i++){
        v = i*2;
        line( x[index[v]],
              y[index[v]],
              x[index[v+1]],
              y[index[v+1]],
              mat[i] );
    }

    return 0;
}

