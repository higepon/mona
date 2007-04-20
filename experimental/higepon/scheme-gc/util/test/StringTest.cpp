#include "StringTest.h"
#include <string.h>
#include "../String.h"
#include "../Vector.h"

CPPUNIT_TEST_SUITE_REGISTRATION(StringTest);

using namespace util;

void StringTest::setUp()
{
}

void StringTest::tearDown()
{
}

void StringTest::testReplace()
{
    String s("Hello, World, Hello\n");

    s.replace("World", "world");
    CPPUNIT_ASSERT(s == "Hello, world, Hello\n");

    s.replace("Hello", "Hi");
    CPPUNIT_ASSERT(s == "Hi, world, Hi\n");
}

void StringTest::testReplaceOnce()
{
    String s("ABC");
    s.replaceOnce("A", "VW");
    CPPUNIT_ASSERT(s == "VWBC");

    s = "Hello, World\n";
    s.replaceOnce("W", "The W");
    CPPUNIT_ASSERT(s == "Hello, The World\n");

    s.replaceOnce("Hello", "Hi");
    CPPUNIT_ASSERT(s == "Hi, The World\n");
}

void StringTest::testIndexOf()
{
    String s("Hello, World\n");
    CPPUNIT_ASSERT(s.indexOf("H") == 0);
    CPPUNIT_ASSERT(s.indexOf("Hello") == 0);
    CPPUNIT_ASSERT(s.indexOf("ello") == 1);
    CPPUNIT_ASSERT(s.indexOf("d") == 11);
    CPPUNIT_ASSERT(s.indexOf("hello") == -1);
}

void StringTest::testStartWith()
{
    String s("Hello, World\n");
    CPPUNIT_ASSERT(s.startWith("Hello"));

    CPPUNIT_ASSERT(!s.startWith("hello"));
    CPPUNIT_ASSERT(!s.startWith("world"));

    CPPUNIT_ASSERT(s.startWith(""));
}

void StringTest::testInclude()
{
    String s("Hello, World\n");
    CPPUNIT_ASSERT(s.include("Hello"));
    CPPUNIT_ASSERT(s.include("llo"));
    CPPUNIT_ASSERT(s.include("World"));
    CPPUNIT_ASSERT(s.include("World\n"));
    CPPUNIT_ASSERT(s.include(", Wo"));

    CPPUNIT_ASSERT(!s.include("hello"));
    CPPUNIT_ASSERT(!s.include("world"));
    CPPUNIT_ASSERT(!s.include("3"));

    CPPUNIT_ASSERT(s.include(""));
}

void StringTest::testNotEqual()
{
    String s = "This";
    CPPUNIT_ASSERT(s != "Thiz");
    CPPUNIT_ASSERT(s != "Ahis");
    CPPUNIT_ASSERT(s != "This ");
}

void StringTest::testData()
{
    String s = "This is";
    s += " test";
    CPPUNIT_ASSERT(strcmp(s.data(), "This is test") == 0);

    s += '.';
    CPPUNIT_ASSERT(strcmp(s.data(), "This is test.") == 0);

    s = "(" + s + ")";
    CPPUNIT_ASSERT(strcmp(s.data(), "(This is test.)") == 0);

    s += s;
    CPPUNIT_ASSERT(strcmp(s.data(), "(This is test.)(This is test.)") == 0);


    for (char c = 'a'; c <= 'z'; c++)
    {
        s += c;
    }
    CPPUNIT_ASSERT(strcmp(s.data(), "(This is test.)(This is test.)abcdefghijklmnopqrstuvwxyz") == 0);
}

void StringTest::testAtParen()
{
    String s = "This is";
    CPPUNIT_ASSERT(s[0] == 'T');
    CPPUNIT_ASSERT(s[1] == 'h');
    CPPUNIT_ASSERT(s[2] == 'i');
    CPPUNIT_ASSERT(s[3] == 's');
    CPPUNIT_ASSERT(s[4] == ' ');
    CPPUNIT_ASSERT(s[5] == 'i');
    CPPUNIT_ASSERT(s[6] == 's');

    s += " ";
    s += '.';
    CPPUNIT_ASSERT(s[0] == 'T');
    CPPUNIT_ASSERT(s[1] == 'h');
    CPPUNIT_ASSERT(s[2] == 'i');
    CPPUNIT_ASSERT(s[3] == 's');
    CPPUNIT_ASSERT(s[4] == ' ');
    CPPUNIT_ASSERT(s[5] == 'i');
    CPPUNIT_ASSERT(s[6] == 's');
    CPPUNIT_ASSERT(s[7] == ' ');
    CPPUNIT_ASSERT(s[8] == '.');
}

void StringTest::testAt()
{
    String s = "This is";
    CPPUNIT_ASSERT(s.at(0) == 'T');
    CPPUNIT_ASSERT(s.at(1) == 'h');
    CPPUNIT_ASSERT(s.at(2) == 'i');
    CPPUNIT_ASSERT(s.at(3) == 's');
    CPPUNIT_ASSERT(s.at(4) == ' ');
    CPPUNIT_ASSERT(s.at(5) == 'i');
    CPPUNIT_ASSERT(s.at(6) == 's');

    s += " ";
    s += '.';
    CPPUNIT_ASSERT(s.at(0) == 'T');
    CPPUNIT_ASSERT(s.at(1) == 'h');
    CPPUNIT_ASSERT(s.at(2) == 'i');
    CPPUNIT_ASSERT(s.at(3) == 's');
    CPPUNIT_ASSERT(s.at(4) == ' ');
    CPPUNIT_ASSERT(s.at(5) == 'i');
    CPPUNIT_ASSERT(s.at(6) == 's');
    CPPUNIT_ASSERT(s.at(7) == ' ');
    CPPUNIT_ASSERT(s.at(8) == '.');
}

void StringTest::testSize()
{
    String s = "This is";
    CPPUNIT_ASSERT(s.size() == 7);

    s += " test";
    CPPUNIT_ASSERT(s.size() == 12);

    s += '.';
    CPPUNIT_ASSERT(s.size() == 13);

    s = "(" + s + ")";
    CPPUNIT_ASSERT(s.size() == 15);

    s += s;
    CPPUNIT_ASSERT(s.size() == 30);


    for (char c = 'a'; c <= 'z'; c++)
    {
        s += c;
    }
    CPPUNIT_ASSERT(s.size() == 56);
}

void StringTest::testAppend()
{
    String s = "This is";
    s += " test";
    CPPUNIT_ASSERT(s == "This is test");

    s += '.';
    CPPUNIT_ASSERT(s == "This is test.");

    s = "(" + s + ")";
    CPPUNIT_ASSERT(s == "(This is test.)");

    s += s;
    CPPUNIT_ASSERT(s == "(This is test.)(This is test.)");


    for (char c = 'a'; c <= 'z'; c++)
    {
        s += c;
    }
    CPPUNIT_ASSERT(s == "(This is test.)(This is test.)abcdefghijklmnopqrstuvwxyz");
}

void StringTest::testSplit()
{
    String path = "/hoge/hige";
    Vector<String>* ss = path.split('/');
    CPPUNIT_ASSERT(ss->size() == 3);
    CPPUNIT_ASSERT(ss->get(0) == "");
    CPPUNIT_ASSERT(ss->get(1) == "hoge");
    CPPUNIT_ASSERT(ss->get(2) == "hige");
}
