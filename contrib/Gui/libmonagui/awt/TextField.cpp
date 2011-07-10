/*
Copyright (c) 2005 bayside
              2011 Higepon : Refactoring and added testability.
                             Added Emacs keybind
                             Added clipboard support
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
                             selected_(false),
                             selectBeginningOffset_(0),
                             _imeManager(new ImeManager),
                             borderColor_(Color::black)
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

    void TextField::backward()
    {
        if (cursor_ > 0) {
            cursor_--;
            repaint();
        }
    }

    void TextField::forward()
    {
        if (cursor_ < text_.length()) {
            cursor_++;
            repaint();
        }
    }

    void TextField::setText(const String& text)
    {
        setTextNoRepaint(text);
        repaint();
    }

    void TextField::setTextNoRepaint(const String & text)
    {
        text_ = text;
        cursor_ = text_.length();
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
        g->setColor(borderColor_);
        g->drawRect(1, 1, w - 2, h - 2);

        int fh = getFontMetrics()->getHeight(getText());
        int fw = getFontMetrics()->getWidth(getText());

        if (selected_ && selectBeginningOffset_ != cursor_) {
            g->setColor(Color::gray);
            int begin = cursor_ < selectBeginningOffset_ ? cursor_ : selectBeginningOffset_;
            int end = cursor_ < selectBeginningOffset_ ? selectBeginningOffset_ : cursor_;
            int headOffset = getFontMetrics()->getWidth(text_.substring(0, begin));
            int selectedWidth = getFontMetrics()->getWidth(text_.substring(begin, end - begin));
            g->fillRect(offx + headOffset, offy + 3, offx + selectedWidth, offy + 12);
        }

        if (getEnabled() == true) {
            g->setColor(getForeground());
        } else {
            g->setColor(Color::gray);
        }
        g->drawString(getText(), this->offx, (h - fh) / 2);
        // キャレット
        if (getFocused() == true && getEnabled() == true) {
            int offsetToCursor = getFontMetrics()->getWidth(text_.substring(0, cursor_));
            g->drawLine(offx + offsetToCursor, offy, offx + offsetToCursor, offy + 12);
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
            deleteBackward();
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

    void TextField::deleteBackward()
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

        // Toggle IME ON/OFF
        if ((keycode == '\\' && modifiers == KeyEvent::VKEY_CTRL) || (keycode == 'o' && modifiers == KeyEvent::VKEY_CTRL)) {
            _imeManager->processEvent(event);
            return;
        }

        if (isImeOn()) {
            _imeManager->processEvent(event);
            return;
        }

        if (event->getModifiers() == KeyEvent::VKEY_ALT) {
            switch(event->getKeycode()) {
            case 'w':
                copy();
                return;
            }
        }


        if (event->getModifiers() == KeyEvent::VKEY_CTRL) {
            switch(event->getKeycode()) {
            case 'a':
                moveBeginningOfLine();
                return;
            case 'e':
                moveEndOfLine();
                return;
            case 'f':
                forward();
                return;
            case 'b':
                backward();
                return;
            case 'c':
                copy();
                return;
            case 'h':
                deleteBackward();
                return;
            case 'd':
                deleteForward();
                return;
            case 'k':
                killLine();
                return;
            case 'y':
                paste();
                return;
            case 'w':
                cut();
                return;
            case ' ':
                toggleSelect();
                return;
            default:
                break;
        }
        }

        switch(keycode) {
        case  KeyEvent::VKEY_BACKSPACE:
            deleteBackward();
            break;
        case KeyEvent::VKEY_LEFT:
            backward();
            break;
        case KeyEvent::VKEY_RIGHT:
            forward();
            break;
        default:
            if (keycode < 128) {
                insertCharacter(keycode);
                repaint();
            }
            break;
        }
    }

    void TextField::cut()
    {
        if (!selected_) {
            return;
        }

        String toCopy;
        if (cursor_ > selectBeginningOffset_) {
            String head = text_.substring(0, selectBeginningOffset_);
            toCopy = text_.substring(selectBeginningOffset_, cursor_ - selectBeginningOffset_);
            String tail = text_.substring(cursor_);
            text_ = head + tail;
            cursor_ = selectBeginningOffset_;
        } else {
            String head = text_.substring(0, cursor_);
            toCopy = text_.substring(cursor_, selectBeginningOffset_ - cursor_);
            String tail = text_.substring(cursor_);
            text_ = head + tail;
        }
        MonAPI::SharedMemory data(toCopy.lengthBytes());
        data.map();
        memcpy(data.data(), toCopy.getBytes(), data.size());
        intptr_t ret = monapi_clipboard_set(data);
        if (ret != M_OK) {
            monapi_warn("clipboard copy failed");
        }
        repaint();
    }

    void TextField::copy()
    {
        if (!selected_) {
            return;
        }

        String toCopy;
        if (cursor_ > selectBeginningOffset_) {
            toCopy = text_.substring(selectBeginningOffset_, cursor_ - selectBeginningOffset_);
        } else {
            toCopy = text_.substring(cursor_, selectBeginningOffset_ - cursor_);
        }
        MonAPI::SharedMemory data(toCopy.lengthBytes());
        data.map();
        memcpy(data.data(), toCopy.getBytes(), data.size());
        intptr_t ret = monapi_clipboard_set(data);
        if (ret != M_OK) {
            monapi_warn("clipboard copy failed");
        }
    }

    void TextField::paste()
    {
        selected_ = false;
        MonAPI::scoped_ptr<MonAPI::SharedMemory> data(monapi_clipboard_get());
        if (data.get() == NULL) {
            return;
        }
        String s((const char*)data->data(), data->size());
        for (int i = 0; i < data->size(); i++) {
            insertCharacter(s.getBytes()[i]);
        }
        repaint();
    }

}
