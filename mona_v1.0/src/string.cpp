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

void* memcpy(void* s1, const void* s2, size_t size) {

    char* p = (char*)s1;
    const char* s = (char*)s2;

    while (size > 0) {
        *p = *s;
        p++;
        s++;
        size--;
    }
    return s1;
}

char* strncpy(char* s1, const char* s2, size_t n) {

    char* p = s1;

    for (; n > 0; n--) {

        if (*s2 == '\0') break;
        *p = *s2;
        p++;
        s2++;
    }

    while (n > 1) {
        n--;
        *p = '\0';
    }
    return s1;
}

char* strtok(char* str, const char* delim) {

    static char* place;
    char* token;
    int   length;

    if (str != NULL) {

        token = str + strspn(str, delim);
    } else {

        if (place[0] == '\0') return NULL;
        token = place + strspn(place, delim);

    }

    length = strcspn(token, delim);

    if (token[length] == '\0') {

        place = token + length;
    } else {

        token[length] = '\0';
        place = token + length + 1;
    }
    return token;
}

size_t strspn(const char* str1, const char* str2) {

    const char* const head = str1;

    for (; *str1; str1++) {

        const char* t;
        for (t = str2; *t != *str1; t++) {

            if (*t == '\0') return str1 - head;
        }
    }
    return str1 - head;
}

size_t strcspn(const char* str1, const char* str2) {

    const char* const head = str1;
    const char* t;
    for (; *str1; str1++) {

        for (t = str2; *t; t++) if (*t == *str1) return str1 - head;
    }
        return str1 - head;
}
