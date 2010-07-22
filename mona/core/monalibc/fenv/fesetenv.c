#include <fenv.h>
#include <string.h>

int fesetenv(const fenv_t *f)
{
  asm volatile("fldenv %0" : : "m"(*f));
  return 0;
}
