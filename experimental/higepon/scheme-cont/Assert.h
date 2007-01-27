#ifndef __ASSERT_H__
#define __ASSERT_H__

#define ASSERT(condition) {if (!(condition)) {printf("ASSERT failure %s:%d: %s\n", __FILE__, __LINE__, #condition);}}

#endif // __ASSERT_H__
