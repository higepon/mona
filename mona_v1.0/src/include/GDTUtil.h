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
