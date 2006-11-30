#ifndef _FILE_SERVER_TEST_
#define _FILE_SERVER_TEST_

#include <cppunit/extensions/HelperMacros.h>

class StringHelperTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(StringHelperTest);
    CPPUNIT_TEST(testSplitOneChar1);
    CPPUNIT_TEST(testSplitOneChar2);
    CPPUNIT_TEST(testSplitOneChar3);
    CPPUNIT_TEST(testSplitOneChar4);
    CPPUNIT_TEST(testSplitMultiChar1);

    CPPUNIT_TEST_SUITE_END();
public:
    void setUp();
    void tearDown();
    void testSplitOneChar1();
    void testSplitOneChar2();
    void testSplitOneChar3();
    void testSplitOneChar4();
    void testSplitMultiChar1();
    void testSplitMultiChar2();

};

#endif // _FILE_SERVER_TEST_
