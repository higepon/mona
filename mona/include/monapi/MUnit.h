/*!
    \file  MUnit.h
    \brief A simple xUnit framework which used internally on MonAPI.

    Copyright (c) 2009 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision: 3946 $
    \date   create:2009/11/14
*/
#include<sys/types.h>

#ifndef _MONAPI_MUNIT_
#define _MONAPI_MUNIT_

#ifdef MUNIT_GLOBAL_VALUE_DEFINED
#define MUNIT_GLOBAL /* */
#define MUNIT_GLOBAL_VAL(v) = (v)
#else
#define MUNIT_GLOBAL extern "C"
#define MUNIT_GLOBAL_VAL(v) /* */
#endif

#define ASSERT_TRUE(condition) {\
    if (!(condition)) {\
        _printf("MUnit:ASSERT_TRUE failure %s:%d: %s\n", __FILE__, __LINE__, #condition);\
        exit(-1);\
    } else {\
        munit_number_of_passed++;\
    }\
}

// EXPECT family never stop on error.
#define EXPECT_TRUE(condition) {\
    if (!(condition)) {\
        _printf("MUnit:EXPECT_TRUE failure %s:%d: %s\n", __FILE__, __LINE__, #condition);\
        munit_number_of_failed++;\
    } else {\
        munit_number_of_passed++;\
    }\
}

#define ASSERT_GT(a, b) ASSERT_TRUE((a) > (b))
#define ASSERT_EQ(a, b) ASSERT_TRUE((a) == (b))
#define EXPECT_GT(a, b) EXPECT_TRUE((a) > (b))
#define EXPECT_EQ(a, b) EXPECT_TRUE((a) == (b))

MUNIT_GLOBAL int munit_number_of_failed MUNIT_GLOBAL_VAL(0);
MUNIT_GLOBAL int munit_number_of_passed MUNIT_GLOBAL_VAL(0);

inline void munit_show_test_results(const char* msg)
{
    if (munit_number_of_failed == 0) {
        _printf("%s test passed %d/%d\n", msg, munit_number_of_passed, munit_number_of_passed);
    } else {
        _printf("%s test failed %d/%d\n", msg, munit_number_of_passed, munit_number_of_passed + munit_number_of_failed);
    }
}

#define TEST_RESULTS(x) munit_show_test_results(#x)

#endif // _MONAPI_MUNIT_
