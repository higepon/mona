#ifndef _ENDS_WITH_TEST_
#define _ENDS_WITH_TEST_

#include <cppunit/extensions/HelperMacros.h>

class EndsWithTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(EndsWithTest);
    CPPUNIT_TEST(testEndsWithOK);
    CPPUNIT_TEST(testEndsWithNG);

    CPPUNIT_TEST_SUITE_END();
public:
    void setUp();
    void tearDown();
    void testEndsWithOK();
    void testEndsWithNG();

};

#endif // _ENDS_WITH_TEST_
