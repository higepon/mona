#include <monalibc/stdio.h>

int putc(int ch, FILE *fp)
{
    return fp == NULL ? putchar(ch) : 0;
}

