#include<Object.h>

#ifndef _HIGEPOS_UTIL_
#define _HIGEPOS_UTIL_

class Point:public Object {

  public:
    Point();
    Point(int x, int y);
    ~Point();
    char* getName();
    int getX();
    int getY();
  private:
    int x_;
    int y_;
};

#endif
