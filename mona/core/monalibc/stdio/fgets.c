#include <monalibc/stdio.h>

char *fgets(char *buf, int n, FILE *fp)
{
    int i;
    if (fp == NULL || fp->_lbfpos >= fp->_bf._size || n < 1) {
        if (n > 0) *buf = '\0';
        return buf;
    }

    for (i = 0; i < n - 1; i++) {
        int ch = fgetc(fp);
        if (ch == '\r') {
            continue;
        } else if (ch == EOF || ch == '\n') {
            buf[i] = '\0';
            return buf;
        }
        buf[i] = ch;
    }
    buf[n - 1] = '\0';
    return buf;
}

