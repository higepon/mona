#include<higeUtil.h>

Point::Point() {
    Point(-5, -1);
}

Point::Point(int x, int y) {

    x_ = x;
    y_ = y;
}

Point::~Point() {
}

int Point::getX() {
    return x_;
}

int Point::getY() {
    return y_;
}

char* Point::getName() {

    static char buf[] = "Point";
    return buf;
}

