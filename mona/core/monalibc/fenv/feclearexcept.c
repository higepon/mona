#include <fenv.h>

int feclearexcept(int e)
{
    fenv_t ft;

    fegetenv(&ft);
    ft.__status &= ~e;
    fesetenv(&ft);

    return 0;
}
