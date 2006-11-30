#include "StringHelper.h"
#include "StringHelperTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(StringHelperTest);

using namespace MonAPI;
using namespace std;

void StringHelperTest::setUp()
{
}

void StringHelperTest::tearDown()
{
}

void StringHelperTest::testSplitMultiChar1()
{
    string text = "a::bb::ccc";
    Strings ss = StringHelper::split("::", text);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("split 3 items", (int)ss.size(), 3);
    CPPUNIT_ASSERT_EQUAL(ss[0], string("a"));
    CPPUNIT_ASSERT_EQUAL(ss[1], string("bb"));
    CPPUNIT_ASSERT_EQUAL(ss[2], string("cc"));
}

void StringHelperTest::testSplitMultiChar2()
{
    string text = "a::bb::ccc::";
    Strings ss = StringHelper::split("::", text);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("split 3 items", (int)ss.size(), 3);
    CPPUNIT_ASSERT_EQUAL(ss[0], string("a"));
    CPPUNIT_ASSERT_EQUAL(ss[1], string("bb"));
    CPPUNIT_ASSERT_EQUAL(ss[2], string("cc"));
}

void StringHelperTest::testSplitOneChar1()
{
    string text = "1\n22\n333";
    Strings ss = StringHelper::split("\n", text);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("split 3 items", (int)ss.size(), 3);
    CPPUNIT_ASSERT_EQUAL(ss[0], string("1"));
    CPPUNIT_ASSERT_EQUAL(ss[1], string("22"));
    CPPUNIT_ASSERT_EQUAL(ss[2], string("333"));
}

void StringHelperTest::testSplitOneChar2()
{
    string text = "333,22,1";
    Strings ss = StringHelper::split(",", text);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("split 3 items", (int)ss.size(), 3);
    CPPUNIT_ASSERT_EQUAL(ss[0], string("333"));
    CPPUNIT_ASSERT_EQUAL(ss[1], string("22"));
    CPPUNIT_ASSERT_EQUAL(ss[2], string("1"));
}

void StringHelperTest::testSplitOneChar3()
{
    string text = "1\n22\n333\n";
    Strings ss = StringHelper::split("\n", text);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("split 3 items", (int)ss.size(), 3);
    CPPUNIT_ASSERT_EQUAL(ss[0], string("1"));
    CPPUNIT_ASSERT_EQUAL(ss[1], string("22"));
    CPPUNIT_ASSERT_EQUAL(ss[2], string("333"));
}

void StringHelperTest::testSplitOneChar4()
{
    string text = "333,22,1,";
    Strings ss = StringHelper::split(",", text);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("split 3 items", (int)ss.size(), 3);
    CPPUNIT_ASSERT_EQUAL(ss[0], string("333"));
    CPPUNIT_ASSERT_EQUAL(ss[1], string("22"));
    CPPUNIT_ASSERT_EQUAL(ss[2], string("1"));
}
