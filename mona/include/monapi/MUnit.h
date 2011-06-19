/*!
    \file  MUnit.h
    \brief A simple xUnit framework which used internally on MonAPI.

    Copyright (c) 2009 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision: 3946 $
    \date   create:2009/11/14
*/

#ifndef _MONAPI_MUNIT_
#define _MONAPI_MUNIT_

#include <sys/types.h>
#include <string>
#include <monapi/MUnit/Probe.h>
#include <monapi/MUnit/Poller.h>

#ifdef MUNIT_GLOBAL_VALUE_DEFINED
#define MUNIT_GLOBAL /* */
#define MUNIT_GLOBAL_VAL(v) = (v)
#else
#define MUNIT_GLOBAL extern "C"
#define MUNIT_GLOBAL_VAL(v) /* */
#endif

class MUnitService
{
private:
    static intptr_t sendClear(uint32_t dest, uint32_t header)
    {
        MessageInfo msg;
        return MonAPI::Message::sendReceive(&msg, dest, header);
    }

public:
    static intptr_t clearInput(uint32_t dest)
    {
        return sendClear(dest, MSG_TEST_CLEAR_INPUT);
    }

    static intptr_t clearOutput(uint32_t dest)
    {
        return sendClear(dest, MSG_TEST_CLEAR_OUTPUT);
    }
};

#define FAIL(msg) fail(msg, __FILE__, __LINE__)

#define SKIP(condition) printf("MUnit:Skipped %s %s:%d %s\n", #condition, __FILE__, __LINE__,  __PRETTY_FUNCTION__); \
                        logprintf("MUnit:Skipped %s %s:%d\n", #condition, __FILE__, __LINE__);

#define ASSERT_TRUE(condition) {\
    if (!(condition)) {\
        printf("MUnit:ASSERT_TRUE %s failure on [%s]:%d: %s\n", __PRETTY_FUNCTION__, __FILE__, __LINE__, #condition); \
        logprintf("MUnit:ASSERT_TRUE %s failure on [%s]:%d: %s\n", __PRETTY_FUNCTION__, __FILE__, __LINE__, #condition); \
        exit(-1);\
    } else {\
        munit_number_of_passed++;\
    }\
}

#define ASSERT_EQ(expected, actual) {                  \
    intptr_t ac = (actual); \
    if (expected != ac) {\
        printf("MUnit:ASSERT_EQ failure on [%s] expected %s, but got %d %s:%d: \n", __PRETTY_FUNCTION__, #expected, ac, __FILE__, __LINE__); \
        logprintf("MUnit:ASSERT_EQ failure on [%s]s expected %s, but got %d %s:%d: \n", __PRETTY_FUNCTION__, #expected, ac, __FILE__, __LINE__); \
        exit(-1);\
    } else {\
        munit_number_of_passed++;\
    }\
}


// EXPECT family never stop on error.
#define EXPECT_TRUE(condition) {\
    if (!(condition)) {\
        printf("MUnit:EXPECT_TRUE failure on [%s]:%d: %s\n", __FILE__, __LINE__, #condition);\
        logprintf("MUnit:EXPECT_TRUE failure on [%s]:%d: %s\n", __FILE__, __LINE__, #condition);\
        munit_number_of_failed++;\
    } else {\
        munit_number_of_passed++;\
    }\
}
#define EXPECT_FALSE(condition) {\
    if ((condition)) {\
        printf("MUnit:EXPECT_FALSE failure on [%s]:%d: %s\n", __FILE__, __LINE__, #condition);\
        logprintf("MUnit:EXPECT_FALSE failure on [%s]:%d: %s\n", __FILE__, __LINE__, #condition);\
        munit_number_of_failed++;\
    } else {\
        munit_number_of_passed++;\
    }\
}

#define EXPECT_EQ(expected, actual) munit_expect_eq(expected, actual, #expected, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define EXPECT_STR_EQ(expected, actual) munit_expect_eq(expected, actual, #expected, __PRETTY_FUNCTION__, __FILE__, __LINE__)

#define EXPECT_STR_EQ_MSG(expected, actual, msg) {                  \
    const char* ac = (actual); \
    if (0 != strcmp(expected, ac)) {\
        printf("MUnit:EXPECT_EQ failure expected %s, but got (%s) %s:%d: msg: %s\n", #expected, ac, __FILE__, __LINE__, (msg)); \
        logprintf("MUnit:EXPECT_EQ failure expected %s, but got (%s) %s:%d: msg: %s\n", #expected, ac, __FILE__, __LINE__, (msg)); \
        munit_number_of_failed++;\
    } else {\
        munit_number_of_passed++;\
    }\
}

#define EXPECT_EQ_MSG(expected, actual, msg) {                  \
    intptr_t ac = (actual); \
    if (expected != ac) {\
        printf("MUnit:EXPECT_EQ failure on [%s] expected %s, but got %d %s:%d: msg: %s\n", __PRETTY_FUNCTION__, #expected, ac, __FILE__, __LINE__, (msg)); \
        logprintf("MUnit:EXPECT_EQ failure on [%s] expected %s, but got %d %s:%d: msg: %s \n", __PRETTY_FUNCTION__, #expected, ac, __FILE__, __LINE__, (msg)); \
        munit_number_of_failed++;\
    } else {\
        munit_number_of_passed++;\
    }\
}


#define ASSERT_GT(a, b) ASSERT_TRUE((a) > (b))
//#define ASSERT_EQ(a, b) ASSERT_TRUE((a) == (b))
#define EXPECT_GT(a, b) EXPECT_TRUE((a) > (b))
//#define EXPECT_EQ(a, b) EXPECT_TRUE((a) == (b))

MUNIT_GLOBAL int munit_number_of_failed MUNIT_GLOBAL_VAL(0);
MUNIT_GLOBAL int munit_number_of_passed MUNIT_GLOBAL_VAL(0);

inline void fail(const char* msg, const char* function, const char* file, int line)
{
    printf("MUnit:FAIL failure on [%s] [%s]:%d: %s\n", function, file, line, msg);
    logprintf("MUnit:FAIL failure on [%s] [%s]:%d: %s\n", function, file, line, msg);
    munit_number_of_failed++;
}


inline void munit_show_test_results(const char* msg)
{
    if (munit_number_of_failed == 0) {
        printf("%s test passed %d/%d\n", msg, munit_number_of_passed, munit_number_of_passed);
        logprintf("%s test passed %d/%d\n", msg, munit_number_of_passed, munit_number_of_passed);
    } else {
        printf("%s test failed %d/%d\n", msg, munit_number_of_passed, munit_number_of_passed + munit_number_of_failed);
        logprintf("%s test failed %d/%d\n", msg, munit_number_of_passed, munit_number_of_passed + munit_number_of_failed);
    }
}

template <typename X, typename Y>
void munit_expect_eq(X expected, Y actual, const char* expectedStr, const char* function, const char* file, int line)
{
    if (expected != (X)actual) {
        printf("MUnit:EXPECT_EQ failure on [%s] expected %s, but got %d %s:%d: \n", function, expectedStr, actual, file, line);
        logprintf("MUnit:EXPECT_EQ failure on [%s] expected %s, but got %d %s:%d: \n", function, expectedStr, actual, file, line);
        munit_number_of_failed++;
    } else {
        munit_number_of_passed++;
    }
}

// template <>
// void munit_expect_eq(uint64_t expected, uint64_t actual, const char* expectedStr, const char* function, const char* file, int line)
// {
//     if (expected != actual) {
//         printf("MUnit:EXPECT_EQ failure on [%s] expected %s, but got %ld %s:%d: \n", function, expectedStr, actual, file, line);
//         logprintf("MUnit:EXPECT_EQ failure on [%s] expected %s, but got %d %s:%d: \n", function, expectedStr, actual, file, line);
//         munit_number_of_failed++;
//     } else {
//         munit_number_of_passed++;
//     }
// }

template <>
void munit_expect_eq(const char* expected, char* actual, const char* expectedStr, const char* function, const char* file, int line)
{
    if (0 != strcmp(expected, actual)) {
        printf("MUnit:EXPECT_EQ failure on [%s] expected %s, but got (%s) %s:%d: \n", function, expectedStr, actual, file, line);
        logprintf("MUnit:EXPECT_EQ failure on [%s] expected %s, but got (%s) %s:%d: \n", function, expectedStr, actual, file, line);
        munit_number_of_failed++;
    } else {
        munit_number_of_passed++;
    }
}

template <>
void munit_expect_eq(const char* expected, const char* actual, const char* expectedStr, const char* function, const char* file, int line)
{
    if (0 != strcmp(expected, actual)) {
        printf("MUnit:EXPECT_EQ failure on [%s] expected %s, but got (%s) %s:%d: \n", function, expectedStr, actual, file, line);
        logprintf("MUnit:EXPECT_EQ failure on [%s] expected %s, but got (%s) %s:%d:%s: \n", function, expectedStr, actual, file, line);
        munit_number_of_failed++;
    } else {
        munit_number_of_passed++;
    }
}

#define ASSERT_EVENTUALLY(probe) assertEventually(probe, __PRETTY_FUNCTION__, __FILE__, __LINE__)

#ifdef MONA

inline void assertEventually(MonAPI::Probe& probe, const char* function, const char* file, int line, int timeoutMillis = 500, int pollDellayMillis = 50)
{
    std::string description;
    if (!MonAPI::Poller(1000, 50).check(probe, description)) {
        fail(description.c_str(), function, file, line);
    } else {
        munit_number_of_passed++;
    }
}

#endif

#define TEST_RESULTS() munit_show_test_results(__FILE__)

#endif // _MONAPI_MUNIT_
