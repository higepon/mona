#include <monalibc/stdio.h>

int fgetc(FILE *fp)
{
    if (fp == NULL || fp->_lbfpos >= fp->_bf._size) return EOF;

    return fp->_bf._base[fp->_lbfpos++];
}

