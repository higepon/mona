/*!
    \file   monaLib.cpp
    \brief  standard lib?

    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/12/02 update:$Date$
*/

#include<monaLib.h>

/*!
    \brief memset

    memset

    \param buf   buffer for set
    \param value value
    \param size  size of memory

    \author HigePon
    \date   create:2002/12/02 update:
*/
void* memset(void* buf, int value, size_t size) {

    char* tmp;

    for (tmp = (char*)buf; size != 0; size--) {
        *tmp++ = value;
    }
    return buf;
}

/*!
    \brief strcpy

    strcpy

    \param to   copy string from to
    \param from copy string from to

    \author HigePon
    \date   create:2002/12/02 update:
*/
char* strcpy(char* to,const char* from) {
    while (*to++ = *from++);
}
