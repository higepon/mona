/*
 *   Copyright (c) 2010  Higepon(Taro Minowa)  <higepon@users.sourceforge.jp>
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
 */

#include <baygui.h>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>

using namespace MonAPI;

static void test_TextField()
{
    TextField t;
    EXPECT_STR_EQ("", t.getText());
    EXPECT_EQ(0, t.getCursor());

    t.insertCharacter('A');
    EXPECT_STR_EQ("A", t.getText());
    EXPECT_EQ(1, t.getCursor());

    t.insertCharacter('B');
    t.insertCharacter('C');
    t.insertCharacter('D');
    EXPECT_EQ(4, t.getCursor());
    EXPECT_STR_EQ("ABCD", t.getText());

    t.deleteCharacter();
    EXPECT_EQ(3, t.getCursor());
    EXPECT_STR_EQ("ABC", t.getText());

    EXPECT_TRUE(t.cursorLeft());
    t.insertCharacter('E');
    EXPECT_STR_EQ("ABEC", t.getText());

    EXPECT_TRUE(t.cursorLeft());
    t.deleteCharacter();
    EXPECT_STR_EQ("AEC", t.getText());

    EXPECT_TRUE(t.cursorRight());
    EXPECT_TRUE(t.cursorRight());
    t.insertCharacter('Z');
    EXPECT_STR_EQ("AECZ", t.getText());
}

static void test_TextField_cursor()
{
    const char* text = "Hello";
    TextField t;
    t.setText(text);

    t.deleteCharacter();
    EXPECT_STR_EQ("Hell", t.getText());
    t.insertCharacter('o');
    EXPECT_STR_EQ("Hello", t.getText());

    for (int i = 0; i < strlen(text); i++) {
        EXPECT_TRUE(t.cursorLeft());
    }
    EXPECT_EQ(false, t.cursorLeft());

    for (int i = 0; i < strlen(text); i++) {
        EXPECT_TRUE(t.cursorRight());
    }
    EXPECT_EQ(false, t.cursorRight());

    t.insertCharacter('Z');
    EXPECT_STR_EQ("HelloZ", t.getText());
}

static void keyPress(TextField& t, int key, int modifier = 0)
{
    KeyEvent e(Event::KEY_PRESSED, NULL, key, modifier);
    t.processEvent(&e);
}

static void keyPressWithControl(TextField& t, int key)
{
    keyPress(t, key, KeyEvent::VKEY_CTRL);
}

static void test_TextField_ime_off()
{
    TextField t;
    keyPress(t, 'a');
    EXPECT_STR_EQ("a", t.getText());

    keyPress(t, 'i');
    EXPECT_STR_EQ("ai", t.getText());
    EXPECT_EQ(2, t.getCursor());

    keyPress(t, KeyEvent::VKEY_BACKSPACE);
    EXPECT_STR_EQ("a", t.getText());
    EXPECT_EQ(1, t.getCursor());

    keyPress(t, KeyEvent::VKEY_LEFT);
    EXPECT_STR_EQ("a", t.getText());
    EXPECT_EQ(0, t.getCursor());

    keyPress(t, 'c');
    EXPECT_STR_EQ("ca", t.getText());
    EXPECT_EQ(1, t.getCursor());

    keyPress(t, KeyEvent::VKEY_RIGHT);
    EXPECT_EQ(2, t.getCursor());
    keyPress(t, 'b');
    EXPECT_STR_EQ("cab", t.getText());
    EXPECT_EQ(3, t.getCursor());
}

static void test_TextField_ime_on()
{
    TextField t;
    EXPECT_EQ(false, t.isImeOn());
    keyPressWithControl(t, '\\');
    EXPECT_TRUE(t.isImeOn());

    keyPress(t, 'a');
    keyPress(t, KeyEvent::VKEY_ENTER);
    EXPECT_STR_EQ("\u3042", t.getText()); // japanese hiragana A

    keyPress(t, 'i');
    keyPress(t, 'u');
    keyPress(t, KeyEvent::VKEY_ENTER);
    EXPECT_STR_EQ("\u3042\u3043\u3044", t.getText()); // japanese hiragana A I U

}

int main(int argc, char* argv[])
{
    test_TextField();
    test_TextField_cursor();
    test_TextField_ime_off();
    test_TextField_ime_on();

    TEST_RESULTS(baygui);
    return 0;
}

