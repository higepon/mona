/*!
    \file   string.cpp
    \brief  standard library

    standard library

    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/12/15 update:$Date$
*/
#include <string.h>

/*!
    \brief memset

    function memset

    \param buf   buffer to set
    \param value value to set
    \param size  size

    \author HigePon
    \date   create:2002/12/15 update:
*/
void *memset(void* buf, int value, size_t size) {

    char *p = (char*)buf;

    while (size > 0) {
        *p = value;
        p++;
        size--;
    }
    return buf;
}

/*!
    \brief strlen

    function strlen

    \param  str target string
    \return length of str

    \author HigePon
    \date   create:2002/12/15 update:
*/
size_t strlen(const char* str) {

    size_t length;
    for (length = 0; str[length] != '\0'; length++);
    return length;
}

/*!
    \brief strcpy

    function strcpy

    \param  to   destination buffer
    \param  from source string
    \return copy of from

    \author HigePon
    \date   create:2002/12/15 update:
*/
char* strcpy(char* to, const char* from) {

    char* p = to;
    while (*from) {
        *p = *from;
        p++;
        from++;
    }
    *p = '\0';
    return to;
}

/*!
    \brief strcmp

    function strcmp

    \param  str1 string to compare
    \param  str2 string to compare
    \return index of charcter

    \author HigePon
    \date   create:2002/12/15 update:
*/
int strcmp(const char* str1, const char* str2) {

    while (*str1 == *str2) {
        str1++;
        str2++;
        if (*str1 == '\0') return 0;
    }
    return (unsigned char)*str1 - (unsigned char)*str2;
}
