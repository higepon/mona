/* -*- coding: utf-8 -*- */

/* this file should be read and written in utf8. */

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

#define EXPECT_EQ_TEXT_FIELD(expectedText, expectedCursor, t) \
    munit_expect_eq(expectedText, t->getText(), #expectedText, __FILE__, __LINE__); \
    munit_expect_eq(expectedCursor, t->getCursor(), #expectedCursor, __FILE__, __LINE__);

class TestingTextField : public TextField
{
public:
    void update(int x, int y, int w, int h)
    {
        // override
        // Don't draw anything.
    }

    void update()
    {
    }

    void repaint()
    {
    }

};

static TextField* createTextField()
{
    Frame* frame = new Frame;
    frame->addNotify();
    TextField* ret = new TestingTextField;
    frame->add(ret);
    return ret;
}

static void destroyTextField(TextField* t)
{
    delete t->getParent();
    delete t;
}

static void test_TextField()
{
    TextField* t = createTextField();
    EXPECT_EQ_TEXT_FIELD("", 0, t);

    t->insertCharacter('A');
    EXPECT_EQ_TEXT_FIELD("A", 1, t);

    t->insertCharacter('B');
    t->insertCharacter('C');
    t->insertCharacter('D');
    EXPECT_EQ_TEXT_FIELD("ABCD", 4, t);

    t->deleteCharacter();
    EXPECT_EQ_TEXT_FIELD("ABC", 3, t);

    EXPECT_TRUE(t->cursorLeft());
    t->insertCharacter('E');
    EXPECT_EQ_TEXT_FIELD("ABEC", 3, t);

    EXPECT_TRUE(t->cursorLeft());
    t->deleteCharacter();
    EXPECT_EQ_TEXT_FIELD("AEC", 1, t);

    EXPECT_TRUE(t->cursorRight());
    EXPECT_TRUE(t->cursorRight());
    t->insertCharacter('Z');
    EXPECT_EQ_TEXT_FIELD("AECZ", 4, t);
    destroyTextField(t);
}

static void test_TextField_cursor()
{
    const char* text = "Hello";
    TextField* t = createTextField();
    t->setText(text);

    t->deleteCharacter();
    EXPECT_STR_EQ("Hell", t->getText());
    t->insertCharacter('o');
    EXPECT_STR_EQ("Hello", t->getText());

    for (size_t i = 0; i < strlen(text); i++) {
        EXPECT_TRUE(t->cursorLeft());
    }
    EXPECT_EQ(false, t->cursorLeft());

    for (size_t i = 0; i < strlen(text); i++) {
        EXPECT_TRUE(t->cursorRight());
    }
    EXPECT_EQ(false, t->cursorRight());

    t->insertCharacter('Z');
    EXPECT_STR_EQ("HelloZ", t->getText());
    destroyTextField(t);
}

static void keyPress(TextField* t, int key, int modifier = 0)
{
    KeyEvent e(Event::KEY_PRESSED, NULL, key, modifier);
    t->processEvent(&e);
}

static void keyPressWithControl(TextField* t, int key)
{
    keyPress(t, key, KeyEvent::VKEY_CTRL);
}

static void test_TextField_ime_off()
{
    TextField* t = createTextField();
    keyPress(t, 'a');
    EXPECT_STR_EQ("a", t->getText());

    keyPress(t, 'i');
    EXPECT_EQ_TEXT_FIELD("ai", 2, t);

    keyPress(t, KeyEvent::VKEY_BACKSPACE);
    EXPECT_EQ_TEXT_FIELD("a", 1, t);

    keyPress(t, KeyEvent::VKEY_LEFT);
    EXPECT_EQ_TEXT_FIELD("a", 0, t);

    keyPress(t, 'c');
    EXPECT_EQ_TEXT_FIELD("ca", 1, t);

    keyPress(t, KeyEvent::VKEY_RIGHT);
    EXPECT_EQ(2, t->getCursor());
    keyPress(t, 'b');
    EXPECT_EQ_TEXT_FIELD("cab", 3, t);
    destroyTextField(t);
}

static void toggleIme(TextField* t)
{
   keyPressWithControl(t, '\\');
}

static void test_TextField_ime_on()
{
    TextField* t = createTextField();
    EXPECT_EQ(false, t->isImeOn());
    toggleIme(t);
    EXPECT_TRUE(t->isImeOn());

    keyPress(t, 'a');
    keyPress(t, KeyEvent::VKEY_ENTER);
    EXPECT_STR_EQ("あ", t->getText()); // japanese hiragana A

    keyPress(t, 'i');
    keyPress(t, 'u');
    keyPress(t, KeyEvent::VKEY_ENTER);
    EXPECT_STR_EQ("あいう", t->getText()); // japanese hiragana A I U
    destroyTextField(t);
}

static void test_TextField_backspace_ime_on()
{
    TextField* t = createTextField();
    toggleIme(t);
    keyPress(t, 'a');
    keyPress(t, 'i');

    keyPress(t, KeyEvent::VKEY_BACKSPACE);
    EXPECT_EQ(0, t->getCursor());
    EXPECT_STR_EQ("", t->getText());

    keyPress(t, KeyEvent::VKEY_ENTER);
    EXPECT_EQ_TEXT_FIELD("あ", 1, t);
    destroyTextField(t);
}

static void test_TextField_backspace_empty_ime_on()
{
    TextField* t = createTextField();
    keyPress(t, 'a');
    keyPress(t, 'b');
    toggleIme(t);
    keyPress(t, 'a');

    keyPress(t, KeyEvent::VKEY_BACKSPACE);
    EXPECT_EQ(2, t->getCursor());
    EXPECT_STR_EQ("ab", t->getText());

    // When ime buffer is empty, backspace goes to Textfield
    keyPress(t, KeyEvent::VKEY_BACKSPACE);
    EXPECT_EQ(1, t->getCursor());
    EXPECT_STR_EQ("a", t->getText());
    destroyTextField(t);
}

static void test_TextField_ime_convert()
{
    TextField* t = createTextField();
    toggleIme(t);
    keyPress(t, 'a');
    keyPress(t, 'i');

    keyPress(t, ' ');
    keyPress(t, KeyEvent::VKEY_ENTER);
    EXPECT_EQ_TEXT_FIELD("逢い", 1, t);
    destroyTextField(t);
}

static void test_TextField_remove_multi_byte_backspace()
{
    TextField* t = createTextField();
    toggleIme(t);
    keyPress(t, 'a');
    keyPress(t, 'i');

    keyPress(t, KeyEvent::VKEY_ENTER);
    EXPECT_EQ_TEXT_FIELD("あい", 2, t);

    toggleIme(t);
    logprintf("HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH");
    keyPress(t, KeyEvent::VKEY_BACKSPACE);
    EXPECT_EQ(1, t->getCursor());
    EXPECT_STR_EQ("あ", t->getText());

    // keyPress(t, KeyEvent::VKEY_BACKSPACE);
    // EXPECT_EQ(0, t->getCursor());
    // EXPECT_STR_EQ("", t->getText());

    destroyTextField(t);
}
int main(int argc, char* argv[])
{
    test_TextField();
    test_TextField_cursor();
    test_TextField_ime_off();
    test_TextField_ime_on();
    test_TextField_backspace_ime_on();
    test_TextField_backspace_empty_ime_on();

    test_TextField_ime_convert();
    test_TextField_remove_multi_byte_backspace();

//    check cursor on enter
//    test_TextField_convert();
//    test_TextField_convert_second_candidate();

    TEST_RESULTS(baygui);
    return 0;
}

