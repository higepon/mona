#include <monalibc/string.h>

char *strdup(const char *s) {
    char *ret = (char *)malloc(strlen(s) + 1);
    strcpy(ret, s);
    return ret;
}

