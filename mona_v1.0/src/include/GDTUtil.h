/*!
    \file  GDTUtil.h
    \brief some functions for GDT set up

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/06/06 update:$Date$
*/
#ifndef _MONA_GDTUTIL_
#define _MONA_GDTUTIL_

/*!
    \struct  segment descripter
*/
typedef struct {
    word limitL;
    word baseL;
    byte baseM;
    byte type;
    byte limitH;
    byte baseH;
} SegDesc;

class GDTUtil {

  public:
    static void setSegDesc(SegDesc* desc, dword base, dword limit, word type);
};

#endif
