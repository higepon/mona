#ifndef __ASSERT_H__
#define __ASSERT_H__

#define ASSERT(cond) if (!cond) { printf("%s:%d: null pointer exception!\n", __FILE__, __LINE__); exit(1); }

#endif // __ASSERT_H__
