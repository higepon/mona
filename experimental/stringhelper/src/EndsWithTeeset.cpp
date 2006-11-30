#include "StringHelper.h"
#include "SplitTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(SplitTest);

using namespace MonAPI;
using namespace std;

void SplitTest::setUp()
{
}

void SplitTest::tearDown()
{
}

void SplitTest::testSplitMultiChar1()
{
    string text = "a::bb::ccc";
    Strings ss = StringHelper::split("::", text);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("split 3 items", (int)ss.size(), 3);
    CPPUNIT_ASSERT_EQUAL(string("a"),   ss[0]);
    CPPUNIT_ASSERT_EQUAL(string("bb"),  ss[1]);
    CPPUNIT_ASSERT_EQUAL(string("ccc"), ss[2]);
}

void SplitTest::testSplitMultiChar2()
{
    string text = "a::bb::ccc::";
    Strings ss = StringHelper::split("::", text);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("split 3 items", (int)ss.size(), 3);
    CPPUNIT_ASSERT_EQUAL(string("a"),   ss[0]);
    CPPUNIT_ASSERT_EQUAL(string("bb"),  ss[1]);
    CPPUNIT_ASSERT_EQUAL(string("ccc"), ss[2]);
}

void SplitTest::testSplitOneChar1()
{
    string text = "1\n22\n333";
    Strings ss = StringHelper::split("\n", text);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("split 3 items", (int)ss.size(), 3);
    CPPUNIT_ASSERT_EQUAL(string("1"),   ss[0]);
    CPPUNIT_ASSERT_EQUAL(string("22"),  ss[1]);
    CPPUNIT_ASSERT_EQUAL(string("333"), ss[2]);
}

void SplitTest::testSplitOneChar2()
{
    string text = "333,22,1";
    Strings ss = StringHelper::split(",", text);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("split 3 items", (int)ss.size(), 3);
    CPPUNIT_ASSERT_EQUAL(string("333"), ss[0]);
    CPPUNIT_ASSERT_EQUAL(string("22"),  ss[1]);
    CPPUNIT_ASSERT_EQUAL(string("1"),   ss[2]);
}

void SplitTest::testSplitOneChar3()
{
    string text = "1\n22\n333\n";
    Strings ss = StringHelper::split("\n", text);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("split 3 items", (int)ss.size(), 3);
    CPPUNIT_ASSERT_EQUAL(string("1"),   ss[0]);
    CPPUNIT_ASSERT_EQUAL(string("22"),  ss[1]);
    CPPUNIT_ASSERT_EQUAL(string("333"), ss[2]);
}

void SplitTest::testSplitOneChar4()
{
    string text = "333,22,1,";
    Strings ss = StringHelper::split(",", text);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("split 3 items", (int)ss.size(), 3);
    CPPUNIT_ASSERT_EQUAL(string("333"), ss[0]);
    CPPUNIT_ASSERT_EQUAL(string("22"),  ss[1]);
    CPPUNIT_ASSERT_EQUAL(string("1"),   ss[2]);
}
