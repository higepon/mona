/*!
    \file  GDTUti.h
    \brief class GDTUtil

    class GDTUtil

    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/12/30 update:$Date$
*/
#ifndef _MONA_GDT_
#define _MONA_GDT_

#include<monaTypes.h>

#pragma pack(2)

/*!
    \struct gdtr
*/
typedef struct {
    word  limit;
    dword base;
} GDTR;

#pragma pack()

/*!
    \struct  gdt
*/
typedef struct {
    word limitL;
    word baseL;
    byte baseM;
    byte type;
    byte limitH;
    byte baseH;
} GDT;


/*!
    GDT util class
    single pattern  applyes the instance of this class
*/
class GDTUtil {

  private:
    GDTUtil();
    ~GDTUtil();
    GDTUtil(const GDTUtil&);
    GDTUtil& operator = (const GDTUtil&);
  public:
    static GDTUtil& instance();
};

#endif
