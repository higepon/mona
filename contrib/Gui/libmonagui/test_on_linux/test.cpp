/*
 *   Copyright (c) 2011  Higepon(Taro Minowa)  <higepon@users.sourceforge.jp>
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <monagui/lang/String.h>

#define logprintf(...) //
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>


using namespace monagui;

static void doStringWork(String& s)
{
    (const char*)s;
    for (int i = 0; i < s.length(); i++) {
        s.charAt(i);
    }
    s += 'a';
    s += "ab";
    s.getBytes();
    unsigned int* p = s.toCharArray();
    delete[] p;
}


static void test_callDefaultConstructor()
{
    String s;
    EXPECT_STR_EQ("", (const char*)s);
    doStringWork(s);
}

static void test_createFromCString()
{
    String s("hello");
    EXPECT_STR_EQ("hello", (const char*)s);
    doStringWork(s);
}

static void test_callCopyConstructor()
{
    String s("hello");
    String another(s);
    EXPECT_STR_EQ("hello", (const char*)another);
    EXPECT_TRUE((const char*)s != (const char*)another);
    doStringWork(s);
    doStringWork(another);
}

static void test_assignmentOperator()
{
    String s;
    s = "hello";
    doStringWork(s);
    s = "hello2";
    EXPECT_STR_EQ("hello2", (const char*)s);
    doStringWork(s);
}

static void test_assignmentSelf()
{
    String s("hello");
    s = s;
}

static void test_plusEqualOperator()
{
    String s("hello");
    s += ' ';
    s += "w";
    s += String("orld");
    EXPECT_STR_EQ("hello world", (const char*)s);
    doStringWork(s);
}

static void test_plusOperator()
{
    String s("hello");
    String t = s + "hello";
    String u = s + t;
    EXPECT_STR_EQ("hellohello", (const char*)t);
    EXPECT_STR_EQ("hellohellohello", (const char*)u);
    doStringWork(s);
}

static void test_equals()
{
    String s("hello");
    String s2("hello");
    EXPECT_TRUE(s.equals(s));
    EXPECT_TRUE(s2.equals(s));
    EXPECT_TRUE(s.equals(s2));
}

static void test_letterCase()
{
    String s("hello");
    EXPECT_STR_EQ("HELLO", (const char*)s.toUpperCase());
    EXPECT_STR_EQ("hello", (const char*)s.toUpperCase().toLowerCase());
}

int main()
{
    test_callDefaultConstructor();
    test_createFromCString();
    test_callCopyConstructor();
    test_assignmentOperator();
    test_assignmentSelf();
    test_plusEqualOperator();
    test_plusOperator();
    test_equals();
    test_letterCase();
    TEST_RESULTS();
    return 0;
}
