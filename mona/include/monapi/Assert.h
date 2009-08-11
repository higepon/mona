#ifndef _MONAPI_ASSERT_
#define _MONAPI_ASSERT_

#include <sys/types.h>

#ifdef DEBUG_MODE
#define onAssertError() {exit(1);}

#define ASSERT(condition) {if (!(condition)) {printf("ASSERT failure %s:%d: %s\n", __FILE__, __LINE__, #condition);onAssertError();}}
#else
#define ASSERT(condition) {}
#endif

#endif
