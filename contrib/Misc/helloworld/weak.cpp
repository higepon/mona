#include <monapi.h>
int weak_func()
{
    syscall_print(__func__);
    return MONA_SUCCESS;
}
