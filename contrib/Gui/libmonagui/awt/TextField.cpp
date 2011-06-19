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
#include "monagui.h"
#include "monagui/awt/ImeManager.h"

namespace monagui {
    TextField::TextField() : text_(""),
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
        cursor_++;
    }

    void TextField::deleteCharacter()
    {
        ASSERT(cursor_ > 0);
        // cursor is at tail.
        if (text_.length() == cursor_) {
            text_ = text_.substring(0, text_.length() - 1);
        } else {
            String head = text_.substring(0, cursor_ - 1);
            String rest = text_.substring(cursor_, text_.length() - cursor_);
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
        // todo pullup
        int width = _imeManager->isOn() ? 100 : 0;
        int height = _imeManager->isOn() ? getHeight() - offy * 2 : 0;
        _imeManager->setBounds(getX() + offx + fw, getY() + offy, width, height);
    }

    bool TextField::isImeOn() const
    {
        return _imeManager->isOn();
    }

    void TextField::processEvent(Event* event)
    {
        if (!getEnabled()) {
            return;
        }

        switch (event->getType() & 0xFFFF) {
        case Event::IME_CHAR:
        {
            int keycode = (event->getType() >> 16) & 0xFFFF;
            accumulateUtf8(((char)(keycode & 0xFF)));
            break;
        }
        case Event::IME_ENDCOMPOSITION:
        {
            String& stringFromIme = getAccumulateUtf8();
            cursor_ += stringFromIme.length();
            insertStringTail(stringFromIme);
            clearAccumulateUtf8();
            repaint();
            break;
        }
        case Event::IME_BACKSPACE:
        {
            backspace();
            break;
        }
        case KeyEvent::KEY_PRESSED:
        {
            processKeyEvent((KeyEvent*)event);
            break;
        }
        case Event::FOCUS_IN:
        case Event::FOCUS_OUT:
        {
            repaint();
            getParent()->processEvent(&this->focusEvent);
            break;
        }
        }
    }

    void TextField::backspace()
    {
        if(cursor_ > 0){
            deleteCharacter();
            repaint();
        }
    }

    void TextField::processKeyEvent(KeyEvent* event)
    {
        int keycode = event->getKeycode();
        int modifiers = event->getModifiers();
        logprintf("TextField keycode=%x\n", keycode);

        // Toggle IME ON/OFF
        if ((keycode == '\\' && modifiers == KeyEvent::VKEY_CTRL) || (keycode == 'o' && modifiers == KeyEvent::VKEY_CTRL)) {
            _imeManager->processEvent(event);
            return;
        }

        if (isImeOn()) {
            _imeManager->processEvent(event);
            return;
        }

        switch(keycode) {
        case  KeyEvent::VKEY_BACKSPACE:
            backspace();
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
            if (keycode < 128) {
                insertCharacter(keycode);
                repaint();
            }
            break;
        }
    }
}
