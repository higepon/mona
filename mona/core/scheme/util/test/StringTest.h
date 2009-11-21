/*!
    \file  StringTest.h
    \brief 

    Copyright (c) 2006-2007 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

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
    CPPUNIT_TEST(testReplaceOnce);
    CPPUNIT_TEST(testSplit);
    CPPUNIT_TEST(testInsert);
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
    void testReplaceOnce();
    void testSplit();
    void testInsert();
};

#endif // _STRING_TEST_H_
