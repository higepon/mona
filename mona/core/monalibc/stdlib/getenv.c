#include <stdlib.h>

char *getenv(const char *name)
{
    if (strcmp(name, "HOME") == 0) {
        return "/USER";
    } else {
        return NULL;
    }
}
