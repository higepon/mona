#ifndef _MONA_UTIL_
#define _MONA_UTIL_

class Point {

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
