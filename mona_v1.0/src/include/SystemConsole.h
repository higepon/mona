/*!
    \file  SystemConsole.h
    \brief class SystemConsole

    class SystemConsole

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/02/03 update:$Date$
*/
#ifndef _MONA_SYSTEM_CONSOLE_
#define _MONA_SYSTEM_CONSOLE_

#include<VirtualConsole.h>

/*!
    abstract class VirtualConsole
*/
class SystemConsole : public VirtualConsole{

  public:
    void printf(const char *, ...);
};


#endif
