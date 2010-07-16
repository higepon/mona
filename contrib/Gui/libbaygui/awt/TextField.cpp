/*
Copyright (c) 2005 bayside
              2010 Higepon : Refactoring and added testability.
Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <monapi/CString.h>
#include "baygui.h"
#include "baygui/awt/ImeManager.h"

namespace baygui {
    TextField::TextField() : text_(String("")),
                             cursor_(0),
                             _imeManager(new ImeManager)
    {
        initialize();
    }

    void TextField::initialize()
    {
        this->offx = 3;
        this->offy = 3;
        this->textEvent.setType(Event::TEXT_CHANGED);
        this->textEvent.setSource(this);
        setBackground(Color::white);
        _imeManager->setParent(this);
        add(_imeManager);
    }

    TextField::~TextField()
    {
    }

    void TextField::insertStringTail(const String& s)
    {
        text_ += s;
    }

    void TextField::insertCharacter(char c)
    {
        // cursor is at tail.
        if (text_.length() == cursor_) {
            text_ += c;
        } else {
            String head = text_.substring(0, cursor_);
            String rest = text_.substring(cursor_, text_.length() - cursor_);
            text_ = head;
            text_ += c;
            text_ += rest;
        }
        logprintf("insertCharacter %d result = %s", c, (const char*)text_);
        cursor_++;
    }

    void TextField::deleteCharacter()
    {
        ASSERT(cursor_ > 0);
        // cursor is at tail.
        if (text_.length() == cursor_) {
            logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
            text_ = text_.substring(0, text_.length() - 1);
            logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
            logprintf("<%s>", (const char*)text_);
        } else {
            logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
            String head = text_.substring(0, cursor_ - 1);
            String rest = text_.substring(cursor_, text_.length() - cursor_);
            logprintf("<%s>", (const char*)head);
            logprintf("<%s>", (const char*)rest);
            text_ = head;
            text_ += rest;
        }
        cursor_--;
    }

    bool TextField::cursorLeft()
    {
        if (cursor_ > 0) {
            cursor_--;
            return true;
        } else {
            return false;
        }
    }

    bool TextField::cursorRight()
    {
        if (cursor_ < text_.length()) {
            cursor_++;
            return true;
        } else {
            return false;
        }
    }


    void TextField::setText(const String& text)
    {
        text_ = text;
        cursor_ = text_.length();
        repaint();
    }

    void TextField::paint(Graphics* g)
    {
        int w = getWidth(), h = getHeight();

        // 外枠
        if (getFocused() == true && getEnabled() == true) {
            g->setColor(0, 128, 255);
            g->drawRect(0, 0, w, h);
        } else {
            g->setColor(getParent()->getBackground());
            g->drawRect(0, 0, w, h);
        }
        // 内枠
        g->setColor(getBackground());
        g->fillRect(1, 1, w - 2, h - 2);
        g->setColor(getForeground());
        g->drawRect(1, 1, w - 2, h - 2);
        // 文字
        int fh = getFontMetrics()->getHeight(getText());
        int fw = getFontMetrics()->getWidth(getText());

        if (getEnabled() == true) {
            g->setColor(getForeground());
        } else {
            g->setColor(Color::gray);
        }
        g->drawString(getText(), this->offx, (h - fh) / 2);

        // キャレット
        if (getFocused() == true && getEnabled() == true) {
            g->drawLine(offx + fw, offy, offx + fw, offy + 12);
        }
//        _imeManager->setBounds(getX() + offx + fw, getY() + offy, getWidth() - offx * 2 - fw, getHeight() - offy * 2);
        _imeManager->setBounds(getX() + offx + fw, getY() + offy, getWidth() - offx * 2 - fw, getHeight() - offy * 2);
//        _imeManager->setBounds(getX() + offx + fw, getY() + offy, getWidth(), getHeight());
        // _imeManager->setForeground(getForeground());
        // _imeManager->setBackground(getBackground());
        // _imeManager->setFocused(getFocused());
        // _imeManager->setFontStyle(getFontStyle());
        // _imeManager->repaint();
        //g->drawLine(offx + textPtr * 8 + 8, offy, offx + textPtr * 8 + 8, offy + 12);
    }

    bool TextField::isImeOn() const
    {
        return _imeManager->isOn();
    }

    void TextField::processEvent(Event* event)
    {
        logprintf("TextField::processEvent this=%x parent=%x\n", this, getParent());
        // 非活性の時はイベントを受け付けない
        if (getEnabled() == false) return;

        if ((event->getType() & 0xFFFF) == Event::IME_CHAR) {
            int keycode = (event->getType() >> 16) & 0xFFFF;
//            insertCharacter((char)(keycode & 0xFF));
            logprintf("accum = <%d>", (keycode & 0xFF));
            accumulateUtf8(((char)(keycode & 0xFF)));
            return;
        } else if (event->getType() == Event::IME_ENDCOMPOSITION) {
            String& stringFromIme = getAccumulateUtf8();
            cursor_ += stringFromIme.length();
            insertStringTail(stringFromIme);
            clearAccumulateUtf8();
            repaint();
            return;
        }

//         // １文字イベント
//         if ((event->getType() & 0xFFFF) == Event::IME_CHAR) {
//             int keycode = (event->getType() >> 16) & 0xFFFF;
//             // 確定イベント
//             if (keycode == KeyEvent::VKEY_ENTER) {
//                 getParent()->processEvent(&this->textEvent);
//                 //Component::dispatchEvent(&textEvent);
//                 // １文字削除
//             } else if (keycode == KeyEvent::VKEY_BACKSPACE) {
//                 //text[strlen(text) - 1] = 0;
// //                _imeManager->deleteCharacter(text);
//                 repaint();
//                 // １文字挿入
//             } else {
//                 insertCharacter((char)(keycode & 0xFF));
//                 repaint();
// //                _imeManager->insertCharacter(text, (char)(keycode & 0xFF));
//             }
//             // 確定イベント
//         } else if (event->getType() == Event::IME_ENDCOMPOSITION) {
//             const char* s = _imeManager->getFixedString();
//             int len = strlen(s);
//             logprintf("fixed len=%d %s %s:%d\n", len, __func__, __FILE__, __LINE__);
//             for (int i = 0; i < len; i++) {
//                 logprintf("[%x]", s[i]);
//             }
//             repaint();
//             // キー押下
//         } else if (event->getType() == Event::KEY_PRESSED) {
//     logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
//             // IMEマネージャに丸投げ
//             _imeManager->processEvent(event);
//     logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
// //            Component::dispatchEvent(event);
//     if (getParent()) {
//             getParent()->processEvent(&this->textEvent);
//     }
//     logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
//             // フォーカス状態変更
//         } else if (event->getType() == Event::FOCUS_IN || event->getType() == Event::FOCUS_OUT) {
//     logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
// //            if (firstpaint == true) {
//                 repaint();
//                 getParent()->processEvent(&this->focusEvent);
// //            }
//         }


        // キー押下
        if (event->getType() == KeyEvent::KEY_PRESSED) {
            processKeyEvent((KeyEvent*)event);
        // フォーカス状態変更
        } else if (event->getType() == Event::FOCUS_IN || event->getType() == Event::FOCUS_OUT) {
            repaint();
            getParent()->processEvent(&this->focusEvent);
        }
    }
}

void TextField::processKeyEvent(KeyEvent* event)
{
    int keycode = event->getKeycode();
    int modifiers = event->getModifiers();

    // Toggle IME ON/OFF
    if (keycode == '\\' && modifiers == KeyEvent::VKEY_CTRL) {
        _imeManager->processEvent(event);
        return;
    }

    if (isImeOn()) {
        _imeManager->processEvent(event);
        return;
    }

    switch(keycode) {
    case  KeyEvent::VKEY_BACKSPACE:
        if (cursor_ > 0) {
            deleteCharacter();
            repaint();
        }
        break;
    case KeyEvent::VKEY_LEFT:
        if (cursorLeft()) {
            repaint();
        }
        break;
    case KeyEvent::VKEY_RIGHT:
        if (cursorRight()) {
            repaint();
        }
        break;
    // case KeyEvent::VKEY_ENTER:
    //     if (getParent()) {
    //         getParent()->processEvent(&this->textEvent);
    //     }
    //     break;
    default:
        ASSERT(keycode < 128);
        insertCharacter(keycode);
        repaint();
        break;
    }
}
