/*!
    \file  SystemConsole.cpp
    \brief class SystemConsole

    class SystemConsole

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/02/03 update:$Date$
*/

#include<SystemConsole.h>
#include<monaVga.h>

/*!
    \brief printf

    \param  str
    \author HigePon
    \date   create:2003/02/03 update:
*/
void SystemConsole::printf(const char *str, ...args) {

    _sys_printf(str, args);
}
