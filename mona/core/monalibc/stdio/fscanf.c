#include <monalibc/stdio.h>
#include <monalibc/stdarg.h>

int fscanf(FILE *fp, const char *format, ...)
{
    int result;
    va_list args;

    if (fp == NULL || 0 != strncmp(fp->_flags, "r", 1)) return 0;

    va_start(args, format);
    result = vsscanf((const char *)&fp->_bf._base[fp->_lbfpos], format, args);
    va_end(args);

    fp->_lbfpos += result;
    return result;
}

