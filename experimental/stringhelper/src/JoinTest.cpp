#include "StringHelper.h"
#include "EndsWithTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(EndsWithTest);

using namespace MonAPI;
using namespace std;

void EndsWithTest::setUp()
{
}

void EndsWithTest::tearDown()
{
}

void EndsWithTest::testEndsWithOK()
{
    CPPUNIT_ASSERT(StringHelper::endsWith("hige.bmp", ".bmp"));
    CPPUNIT_ASSERT(StringHelper::endsWith("abc", "abc"));
    CPPUNIT_ASSERT(StringHelper::endsWith("abc", "c"));
}

void EndsWithTest::testEndsWithNG()
{
    CPPUNIT_ASSERT(false == StringHelper::endsWith("hige.bmp", "bm"));
    CPPUNIT_ASSERT(false == StringHelper::endsWith("", "b"));
    CPPUNIT_ASSERT(false == StringHelper::endsWith("hige.bmp", "bm"));
    CPPUNIT_ASSERT(false == StringHelper::endsWith("", ""));
}
