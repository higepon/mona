#include<higeUtil.h>

Point::Point() {
    x_ = 10;
    y_ = -777;
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

