#include <monalibc/stdio.h>

int putchar(int ch)
{
    char s[] = { ch, '\0' };
    printf(s);
    return ch;
}

