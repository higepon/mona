#include "MessageUtilTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(MessageUtilTest);

void MessageUtilTest::setUp()
{
    util_ = new MessageUtil;
}

void MessageUtilTest::tearDown()
{
    delete util_;
}

void MessageUtilTest::testCharTo5bit()
{
    CPPUNIT_ASSERT_EQUAL_MESSAGE("A", 0, (int)util_->charTo5bit('A'));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("a", 0, (int)util_->charTo5bit('a'));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("C", 2, (int)util_->charTo5bit('C'));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Z", 25, (int)util_->charTo5bit('Z'));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("1", 26, (int)util_->charTo5bit('1'));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("5", 30, (int)util_->charTo5bit('5'));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("space", 31, (int)util_->charTo5bit(' '));

    CPPUNIT_ASSERT_EQUAL_MESSAGE("over 5 == space", 0, (int)util_->charTo5bit('6'));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("!", 0, (int)util_->charTo5bit('!'));
}

void MessageUtilTest::testStringToDword()
{
    CPPUNIT_ASSERT_EQUAL_MESSAGE("NULL", (dword)0, util_->stringToDword(NULL));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("A", (dword)0, util_->stringToDword("A"));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("AAA:AAA", (dword)2, util_->stringToDword("AAA:AAA"));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("AAA AAA", (dword)0, util_->stringToDword("AAA AAA"));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("NET:SND", (dword)0x6927268e, util_->stringToDword("NET:SND"));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("net:snd", (dword)0x6927268e, util_->stringToDword("net:snd"));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("FIL OPN", (dword)0x2a16e7b4, util_->stringToDword("FIL OPN"));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("NET RCV", (dword)0x69271156, util_->stringToDword("NET:RCV"));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("NET RCV reply", (dword)0x69271157, util_->stringToDword("NET:RCV", true));
}
