/*!
    \file   string.cpp
    \brief  standard library

    standard library

    Copyright (c) 2002,2003 Higepon
    All rights reserved.
    License=MIT/X Licnese

    \author  HigePon
    \version $Revision$
    \date   create:2002/12/15 update:$Date$
*/
#include <monapi/string.h>

/*!
    \brief memmove
*/
void *memmove(void* s1, const void* s2, size_t size)
{
    void *p = s1;
    char *c1 = (char*)s1;
    char *c2 = (char*)s2;

    if (c1 > c2) {
        c1 += size;
        c2 += size;
        while (0 < size--)
            *c1-- = *c2--;
    } else {
        while (0 < size--)
            *c1++ = *c2++;
    }

    return p;
}

/*!
    \brief memcmp
*/
int memcmp(const void* s1, const void* s2, size_t size)
{
    char *c1 = (char*)s1;
    char *c2 = (char*)s2;

    while (0 < size--) {
        if (*c1 != *c2)
            return *c1 - *c2;
        c1++;
        c2++;
    }

    return 0;
}

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
        if (*str1 == '\0') return 0;
        str1++;
        str2++;
    }
    return (unsigned char)*str1 - (unsigned char)*str2;
}

void* memcpy(void* s1, void* s2, size_t size) {

//     slow?
//     char* p = (char*)s1;
//     const char* s = (char*)s2;

//     while (size > 0) {
//         *p = *s;
//         p++;
//         s++;
//         size--;
//     }

    asm volatile("movl %0, %%edi \n"
                 "movl %1, %%esi \n"
                 "movl %2, %%ecx \n"
                 "cld            \n"
                 "rep            \n"
                 "movsb          \n"
                 : /* no output */
                 : "m"(s1), "m"(s2), "m"(size)
                 : "edi", "esi", "ecx");

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

char* ltona(long value, char* str, int n, int base) {

    static const char xdigit[] = {"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
    unsigned long ulvalue;
    unsigned int ubase;
    int sign = 0;
    char* ps = str + n;

    *ps = '\0';
    if (base < 0) {
        ubase = -base;
        if (value < 0) {
            value = -value;
            sign  = -1;
            --n;
        }
    } else {
        ubase = base;
    }

    ulvalue  = value;
    if (n > 0 && (2 <= ubase && ubase <  sizeof(xdigit))) {
        do {
            *--ps = xdigit[(unsigned)(ulvalue % ubase)];
        } while (--n > 0 && (ulvalue /= ubase) != 0);
    }
    if (sign < 0)
        *--ps = '-';
    while (--n >= 0)
        *--ps = '0';
    return str;
}

char *strcat(char *s,char *t) {

    char* p = s + strlen(s);
    while ((*p++ = *t++) != 0);
    return s;
}

/*!
    \brief strstr

    \author Tino
    \date   create:2004/03/18 update:
*/
char* strstr(const char* str1, const char* str2)
{
    if (str1 == NULL || str2 == NULL) return NULL;
    for (const char* p1 = str1; *p1 != '\0'; p1++) {
        for (const char* p1a = p1, * p2 = str2;; p1a++, p2++) {
            if (*p2 == '\0') return (char*)p1;
            if (*p2 != *p1a) break;
        }
    }
    return NULL;
}

int strncmp(const char* s1, const char* s2, size_t n)
{

    if (n == 0) return 0;

    do
    {
        if (*s1 != *s2++)
        {
            return (*(const unsigned char *)s1 -  *(const unsigned char *)(s2 - 1));
        }

        if (*s1++ == 0)
        {
            break;
        }
    } while (--n != 0);
    return 0;
}
