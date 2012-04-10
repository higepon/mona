#include <stdlib.h>

char *getenv(const char *name)
{
    if (strcmp(name, "HOME") == 0) {
        return "/USER";
    } else if (strcmp(name, "MOSH_LOADPATH") == 0) {
      return "/USER/LIB";
    } else if (strcmp(name, "DEBUG_CURL") == 0) {
      return "TRUE";
    } else {
        return NULL;
    }
}
