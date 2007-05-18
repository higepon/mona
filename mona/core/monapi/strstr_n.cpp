#include <monapi/strstr_n.h>

char* MonAPI::strstr_n(const char* str1, const char* str2, int size)
{
    if (str1 == NULL || str2 == NULL) return NULL;
    int i = 0;
    for (const char* p1 = str1; i < size; p1++, i++) {
        for (const char* p1a = p1, * p2 = str2;; p1a++, p2++, i++) {
            if (*p2 == '\0') return (char*)p1;
            if (*p2 != *p1a) break;
        }
    }
    return NULL;
}
