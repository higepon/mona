#ifndef _TMPMONA_LIB_ASSERT_
#define _TMPMONA_LIB_ASSERT_

#include <monapi/Assert.h>

#ifdef NDEBUG
#define assert(p)	((void)0)
#else
#define assert(e)	ASSERT(e)
#endif

#endif
