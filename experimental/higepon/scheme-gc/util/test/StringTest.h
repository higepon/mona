#ifndef _STRING_TEST_H_
#define _STRING_TEST_H_

#include <cppunit/extensions/HelperMacros.h>

class StringTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(StringTest);
    CPPUNIT_TEST(testAppend);
    CPPUNIT_TEST(testSize);
    CPPUNIT_TEST(testAt);
    CPPUNIT_TEST(testAtParen);
    CPPUNIT_TEST(testData);
    CPPUNIT_TEST(testNotEqual);
    CPPUNIT_TEST(testInclude);
    CPPUNIT_TEST(testStartWith);
    CPPUNIT_TEST(testIndexOf);
    CPPUNIT_TEST(testReplace);
    CPPUNIT_TEST_SUITE_END();
public:
    void setUp();
    void tearDown();
    void testAppend();
    void testSize();
    void testAt();
    void testAtParen();
    void testData();
    void testNotEqual();
    void testInclude();
    void testStartWith();
    void testIndexOf();
    void testReplace();
};

#endif // _STRING_TEST_H_
