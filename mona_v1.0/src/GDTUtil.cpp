/*!
    \file  GDTUtil.cpp
    \brief class GDTUtil

    class GDTUtil

    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/12/30 update:$Date$
*/
#include<GDTUtil.h>

/*!
    \brief destructor

    destructor

    \author HigePon
    \date   create:2002/12/30 update:
*/
GDTUtil::~GDTUtil() {
}

/*!
    \brief private constructor

    constructor

    \author HigePon
    \date   create:2002/12/30 update:
*/
GDTUtil::GDTUtil() {

}

/*!
    \brief get the instance of this class

    get the instance of this class
    this class has no public constructor.

    \author HigePon
    \date   create:2002/12/30 update:
*/
GDTUtil& GDTUtil::instance() {
    static GDTUtil theInstance;
    return theInstance;
}
