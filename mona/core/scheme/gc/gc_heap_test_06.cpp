/*!
    \file   gc_heap_test_06.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "gc_helper.h"
#include "../util/Vector.h"

using namespace util;

Vector<int*> v;

void test()
{
    v.add(new int);
}

int main(int argc, char *argv[])
{
    gc_init();
    test();
    gc();
    int expected[] = {4, 20};
    show_gc_result(expected, sizeof(expected) /sizeof(int), argv[0]);
}
