#include<Object.h>

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
