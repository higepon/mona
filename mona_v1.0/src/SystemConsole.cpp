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

    \param  format
    \author HigePon
    \date   create:2003/02/03 update:
*/
void SystemConsole::printf(const char *format, ...) {

    void** list = (void **)&format;

    ((char**)list) += 1;
    for (int i = 0; format[i] != '\0'; i++) {

        if (format[i] == '%') {
            i++;

            switch (format[i]) {
              case 's':
                  _sysPrint((char *)*list);
                  ((char**)list) += 1;
                  break;
              case 'd':
                  _sysPrintInt((int)*list);
                  ((int*)list) += 1;
                  break;
              case 'x':
                  _sysPrint("0x");
                  _sysPutInt((int)*list, 16);
                  ((int*)list) += 1;
                  break;
              case 'c':
                  _sysPutCharacter((char)*list);
                  ((char*)list) += 1;
                  break;
              case '%':
                  _sysPutCharacter('%');
                  break;
              case '\0':
                  i--;
                  break;
            }
        } else {
            _sysPutCharacter(format[i]);
        }
    }
}
