#include <string.h>

void *memset(void* buf, int value, size_t size) {

    char *p = (char*)buf;

    while (size > 0) {
        *p = value;
        p++;
        size--;
    }
    return buf;
}

size_t strlen(const char* str) {

    size_t length;
    for (length = 0; str[length] != '\0'; length++);
    return length;
}

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

int strcmp(const char* str1, const char* str2) {

    while (*str1 == *str2) {
        str1++;
        str2++;
        if (*str1 == '\0') return 0;
    }
    return (unsigned char)*str1 - (unsigned char)*str2;
}
