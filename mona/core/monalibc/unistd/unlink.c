#include <sys/error.h>
#include <monalibc/errno.h>
#include <stdint.h>
#include <monapi/messages.h>

int unlink(const char* path)
{
    intptr_t ret = monapi_file_delete(path);
    if (ret == M_OK) {
        return 0;
    } else {
        errno = ENOENT;
        return -1;
    }
}
