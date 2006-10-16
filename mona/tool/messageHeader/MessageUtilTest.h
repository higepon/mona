#ifndef __MESSAGEUTILTEST_H__
#define __MESSAGEUTILTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include "MessageUtil.h"

class MessageUtilTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(MessageUtilTest);
    CPPUNIT_TEST(testCharTo5bit);
    CPPUNIT_TEST(testStringToDword);
    CPPUNIT_TEST_SUITE_END();
public:
    void setUp();
    void tearDown();
    void testCharTo5bit();
    void testStringToDword();

private:
    MessageUtil* util_;
};

#endif // __MESSAGEUTILTEST_H__
