/*!
    \file   info.cpp
    \brief  information out function

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/05/25 update:$Date$
*/
#include<global.h>
#include<GraphicalConsole.h>
#include<info.h>

void info(int level, const char *format, ...) {

    /* do nothing */
    if (level > g_info_level) return;

    /* out */
    g_console->printf("[%s]:", g_process_name);

    void** list = (void **)&format;

    ((char**)list) += 1;
    for (int i = 0; format[i] != '\0'; i++) {

        if (format[i] == '%') {
            i++;

            switch (format[i]) {
              case 's':
                  g_console->print((char *)*list);
                  ((char**)list) += 1;
                  break;
              case 'd':
                  g_console->printInt((int)*list);
                  ((int*)list) += 1;
                  break;
              case 'x':
                  g_console->print("0x");
                  g_console->putInt((int)*list, 16);
                  ((int*)list) += 1;
                  break;
              case 'c':
                  g_console->putCharacter((char)*list);
                  ((char*)list) += 1;
                  break;
              case '%':
                  g_console->putCharacter('%');
                  break;
              case '\0':
                  i--;
                  break;
            }
        } else {
            g_console->putCharacter(format[i]);
        }
    }
}











