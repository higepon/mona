#include <fenv.h>

int fegetenv(fenv_t *f){
	
	asm volatile("fstenv %0" : "=m"(*f));
	return 0;
	
}
