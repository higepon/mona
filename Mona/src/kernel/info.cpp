/*!
    \file   info.cpp
    \brief  information out function

    Copyright (c) 2003 Higepon
    All rights reserved.
    License=MIT/X Licnese

    \author  HigePon
    \version $Revision$
    \date   create:2003/05/25 update:$Date$
*/
#include<global.h>
#include<GraphicalConsole.h>
#include<info.h>

void info(int level, const char *format, ...) {

    static bool msg_complete = true;
    int i;

    /* do nothing */
    if (level > g_info_level) return;

    if (level == ERROR) g_console->setCHColor(GP_RED);
    else if (level == WARNING) g_console->setCHColor(GP_YELLOW);

    /* out */
    //    if (msg_complete && *g_current_process->name) g_console->printf("[%s]:", g_current_process->name);

    void** list = (void **)&format;

    ((char**)list) += 1;
    for (i = 0; format[i] != '\0'; i++) {

        if (format[i] == '%') {
            i++;

            switch (format[i]) {
              case 's':
                  g_console->printf("%s", (char *)*list);
                  ((char**)list) += 1;
                  break;
              case 'd':
                  g_console->printf("%d", (int)*list);
                  ((int*)list) += 1;
                  break;
              case 'x':
                  g_console->printf("%x", (int)*list, 16);
                  ((int*)list) += 1;
                  break;
              case 'c':
                  g_console->printf("%c", ((char)(int)(*list)));
                  ((char*)list) += 1;
                  break;
              case '%':
                  g_console->printf("%%");
                  break;
              case '\0':
                  i--;
                  break;
            }
        } else {
            g_console->printf("%c", (format[i]));
        }
    }

    msg_complete = format[i -1] == '\n';

    if (level == ERROR || level == WARNING) g_console->setCHColor(GP_WHITE);
}
