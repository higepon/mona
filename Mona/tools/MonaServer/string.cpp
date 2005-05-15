/*!
    \file   string.cpp
    \brief  standard library

    standard library

    Copyright (c) 2002,2003 Higepon and the individuals listed on the ChangeLog entries.
    All rights reserved.
    License=MIT/X License

    \author  HigePon
    \version $Revision$
    \date   create:2002/12/15 update:$Date$
*/

#include <stdafx.h>

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
