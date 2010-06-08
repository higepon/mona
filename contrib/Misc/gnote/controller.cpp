/*
 Copyright (c) 2005 hetappi
 All rights reserved.

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to
 deal in the Software without restriction, including without limitation the
 rights to use, copy, modify, merge, publish, distribute, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to
 do so, provided that the above copyright notice(s) and this permission
 notice appear in all copies of the Software and that both the above
 copyright notice(s) and this permission notice appear in supporting
 documentation.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.
 IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS NOTICE BE
 LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR
 ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

 Except as contained in this notice, the name of a copyright holder shall not
 be used in advertising or otherwise to promote the sale, use or other
 dealings in this Software without prior written authorization of the
 copyright holder.
*/
#include "controller.hpp"

namespace gnote {
    Controller::Controller() : keyCode(0), keyModifiers(0), counter(0) {
    }
    String Controller::ToCanvasString(const String& s) {
        return window.GetCanvas()->ToCanvasString(s);
    }
    String Controller::ToCanvasString(const Document& d, int wy) {
        return window.GetCanvas()->ToCanvasString(d, wy);
    }
    int Controller::ToCanvasY(int wy, int t) {
        return window.GetCanvas()->ToCanvasY(wy, t);
    }
    int Controller::ToCanvasX(int wy, int wx, int l) {
        return window.GetCanvas()->ToCanvasX(wy, wx, l);
    }
    int Controller::ToWorldX(int wy, int cx, int l) {
        return window.GetCanvas()->ToWorldX(wy, cx, l);
    }
    //
    void Controller::ProcessEvent(Event* event) {
        switch(event->getType()) {
            case Event::TIMER:
                ProcessTimerEvent(reinterpret_cast<TimerEvent*>(event));
                break;
            case Event::KEY_PRESSED:
                if (!window.IsPopuped()) {
                    ProcessKeyPressedEvent(reinterpret_cast<KeyEvent*>(event));
                }
                break;
            case Event::ACTION_PERFORMED:
                ProcessActionEvent(reinterpret_cast<TActionEvent*>(event));
                break;
            default:
                break;
        }
    }
    //
    void Controller::ProcessActionEvent(TActionEvent* event) {
        if (!event) {
            return;
        }
        String command = event->getActionCommand();
        //
        // File
        //
        if (command.equals(String("FILE_NEW"))) {
            if (New(cursol, document)) {
                cursol.visible = true;
                window.GetCanvas()->repaint();
            }
        } else if (command.equals(String("FILE_QUIT"))) {
            Exit();
        } else if (command.equals(String("FILE_SAVE"))) {
            WriteFile(file, document);
        }
        //
        // Edit
        //
        else if (command.equals(String("EDIT_CUT"))) {
            if (Cut(cursol, document, clip)) {
                cursol.visible = true;
                window.GetCanvas()->repaint();
            }
        } else if (command.equals(String("EDIT_COPY"))) {
            if (Copy(cursol, document, clip)) {
                cursol.visible = true;
                window.GetCanvas()->repaint();
            }
        } else if (command.equals(String("EDIT_PASTE"))) {
            if (Paste(cursol, document, clip)) {
                cursol.visible = true;
                window.GetCanvas()->repaint();
            }
        } else if (command.equals(String("EDIT_DELETE"))) {
            if (Delete(cursol, document)) {
                cursol.visible = true;
                window.GetCanvas()->repaint();
            }
        } else if (command.equals(String("EDIT_SELECTALL"))) {
            cursol.range = true;
            cursol.rx = cursol.rx = 1;
            if (GoTail(cursol, document)) {
                cursol.visible = true;
                window.GetCanvas()->repaint();
            }
        }
    }
    //
    void Controller::ProcessTimerEvent(TimerEvent* event) {
        if (keyCode) {
            switch(keyCode){
                case KeyEvent::VKEY_PGUP:
                    SetCursolRange(cursol, keyModifiers);
                    if (PageUp(cursol, document)) {
                        cursol.visible = true;
                        window.GetCanvas()->repaint();
                    }
                    keyCode = 0;
                    break;
                case KeyEvent::VKEY_UP:
                    SetCursolRange(cursol, keyModifiers);
                    if (GoUp(cursol, document)) {
                        cursol.visible = true;
                        window.GetCanvas()->repaint();
                    }
                    keyCode = 0;
                    break;
                case KeyEvent::VKEY_PGDOWN:
                    SetCursolRange(cursol, keyModifiers);
                    if (PageDown(cursol, document)) {
                        cursol.visible = true;
                        window.GetCanvas()->repaint();
                    }
                    keyCode = 0;
                    break;
                case KeyEvent::VKEY_DOWN:
                    SetCursolRange(cursol, keyModifiers);
                    if (GoDown(cursol, document)) {
                        cursol.visible = true;
                        window.GetCanvas()->repaint();
                    }
                    keyCode = 0;
                    break;
                case KeyEvent::VKEY_RIGHT:
                    SetCursolRange(cursol, keyModifiers);
                    if (GoRight(cursol, document)) {
                        cursol.visible = true;
                        window.GetCanvas()->repaint();
                    }
                    keyCode = 0;
                    break;
                case KeyEvent::VKEY_LEFT:
                    SetCursolRange(cursol, keyModifiers);
                    if (GoLeft(cursol, document)) {
                        cursol.visible = true;
                        window.GetCanvas()->repaint();
                    }
                    keyCode = 0;
                    break;
                case KeyEvent::VKEY_DELETE:
                    if (Delete(cursol, document)) {
                        cursol.visible = true;
                        window.GetCanvas()->repaint();
                    }
                    break;
                case KeyEvent::VKEY_BACKSPACE:
                    if (BackSpace(cursol, document)) {
                        cursol.visible = true;
                        window.GetCanvas()->repaint();
                    }
                    break;
            }
            keyCode = 0;
        }

        //
        window.setTimer(TIMER_INTERVAL);
        counter += TIMER_INTERVAL;
        if (counter > BLINK_TIMER_INTERVAL) {
            cursol.visible = !cursol.visible;
            // hum
            if (!window.IsPopuped()) {
                window.GetCanvas()->repaintAt(ToCanvasY(cursol.wy, cursol.top));
            }
            counter = 0;
        }
    }

#define EMACS_KEY_BIND
    //
    void Controller::ProcessKeyPressedEvent(KeyEvent* event) {
        int code = event->getKeycode();
        bool done = false;
        switch(event->getModifiers()) {
            case KeyEvent::VKEY_LSHIFT:
            case KeyEvent::VKEY_RSHIFT:
                break;
            case KeyEvent::VKEY_ALT:
                done = true;
                break;
            case KeyEvent::VKEY_CTRL:
                switch(code) {
#ifdef EMACS_KEY_BIND
                    case 'e':
                        if (GoEnd(cursol, document)) {
                            cursol.visible = true;
                            window.GetCanvas()->repaint();
                        }
                        break;
                    case 'a':
                        if (GoHome(cursol, document)) {
                            cursol.visible = true;
                            window.GetCanvas()->repaint();
                        }
                        break;
                    case 'h':
                        if (BackSpace(cursol, document)) {
                            cursol.visible = true;
                            window.GetCanvas()->repaint();
                        }
                        break;
                    case 'f':
                        if (GoRight(cursol, document)) {
                            cursol.visible = true;
                            window.GetCanvas()->repaint();
                        }
                        break;
                    case 'b':
                        if (GoLeft(cursol, document)) {
                            cursol.visible = true;
                            window.GetCanvas()->repaint();
                        }
                        break;
                    case 'n':
                        if (GoDown(cursol, document)) {
                            cursol.visible = true;
                            window.GetCanvas()->repaint();
                        }
                        break;
                    case 'p':
                        if (GoUp(cursol, document)) {
                            cursol.visible = true;
                            window.GetCanvas()->repaint();
                        }
                        break;
#else
                    case 'a':
                        cursol.range = true;
                        cursol.rx = cursol.ry = 1;
                        if (GoTail(cursol, document)) {
                            cursol.visible = true;
                            window.GetCanvas()->repaint();
                        }
                        break;
                    case 'n':
                        if (New(cursol, document)) {
                            cursol.visible = true;
                            window.GetCanvas()->repaint();
                        }
                        break;
#endif
                    case 'v':
                        if (Paste(cursol, document, clip)) {
                            cursol.visible = true;
                            window.GetCanvas()->repaint();
                        }
                        break;
                    case 'x':
                        if (Cut(cursol, document, clip)) {
                            cursol.visible = true;
                            window.GetCanvas()->repaint();
                        }
                        break;
                    case 'c':
                        if (Copy(cursol, document, clip)) {
                            cursol.visible = true;
                            window.GetCanvas()->repaint();
                        }
                        break;
                    case KeyEvent::VKEY_HOME:
                        SetCursolRange(cursol, keyModifiers);
                        if (GoHead(cursol, document)) {
                            cursol.visible = true;
                            window.GetCanvas()->repaint();
                        }
                        break;
                    case KeyEvent::VKEY_END:
                        SetCursolRange(cursol, keyModifiers);
                        if (GoTail(cursol, document)) {
                            cursol.visible = true;
                            window.GetCanvas()->repaint();
                        }
                        break;
                    default:
                        break;
                }
                done = true;
                break;
            default:
                break;
        }

        if (!done) {
            switch(code){
                case KeyEvent::VKEY_UP:
                case KeyEvent::VKEY_DOWN:
                case KeyEvent::VKEY_RIGHT:
                case KeyEvent::VKEY_LEFT:
                case KeyEvent::VKEY_BACKSPACE:
                case KeyEvent::VKEY_DELETE:
                case KeyEvent::VKEY_PGUP:
                case KeyEvent::VKEY_PGDOWN:
                    keyCode = code;
                    keyModifiers = event->getModifiers();
                    break;
                case KeyEvent::VKEY_HOME:
                    SetCursolRange(cursol, event->getModifiers());
                    if (GoHome(cursol, document)) {
                        cursol.visible = true;
                        window.GetCanvas()->repaint();
                    }
                    break;
                case KeyEvent::VKEY_END:
                    SetCursolRange(cursol, event->getModifiers());
                    if (GoEnd(cursol, document)) {
                        cursol.visible = true;
                        window.GetCanvas()->repaint();
                    }
                    break;
                case KeyEvent::VKEY_TAB:
                    if (document.Insert('\t', cursol.wy, cursol.wx)) {
                        GoRight(cursol, document);
                        cursol.visible = true;
                        window.GetCanvas()->repaint();
                    }
                    break;
                case KeyEvent::VKEY_ENTER:
                    if (document.Insert('\n', cursol.wy, cursol.wx)) {
                        GoDown(cursol, document);
                        GoHome(cursol, document);
                        cursol.visible = true;
                        window.GetCanvas()->repaint();
                    }
                    break;
                default:
                    if (code >= 0x20 && code <= 0x7f) {
                        if (document.Insert(code, cursol.wy, cursol.wx)) {
                            GoRight(cursol, document);
                            cursol.visible = true;
                            window.GetCanvas()->repaint();
                        }
                    }
                    break;
            }
        }
    }
    //
    bool Controller::GoHome(Cursol& c, const Document& d) {
        c.left = c.wx = 1;
        return true;
    }
    //
    bool Controller::GoEnd(Cursol& c, const Document& d) {
        String* s = d.GetLine(c.wy);
        return Go(c, d, c.wy, s ? s->length() + 1: 1);
    }
    //
    bool Controller::GoHead(Cursol& c, const Document& d) {
        c.top = c.wy = c.left = c.wx = 1;
        return true;
    }
    //
    bool Controller::GoTail(Cursol& c, const Document& d) {
        String* s = d.GetLine(d.GetMaxLineNumber());
        return Go(c, d, d.GetMaxLineNumber() + 1, s ? s->length() + 1 : 1);
    }
    //
    bool Controller::GoRight(Cursol& c, const Document& d, int l) {
        bool r = false;
        for (int ii = 0; ii < l; ii++) {
            if (c.wy <= d.GetMaxLineNumber()) {
                String* s = d.GetLine(c.wy);
                if ((s->endsWith("\n") && c.wx < s->length()) || (!s->endsWith("\n") && c.wx < s->length() + 1)) {
                    if (ToCanvasX(c.wy, c.wx, c.left) > (Canvas::MAX_CANVASX - 3)) {
                        c.left++;
                    }
                    c.wx++;
                } else {
                    GoDown(c, d) && GoHome(c, d);
                }
                r = true;
            }
        }
        return r;
    }
    //
    bool Controller::GoLeft(Cursol& c, const Document& d, int l) {
        bool r = false;
        for (int ii = 0; ii < l; ii++) {
            if (c.wx > 1) {
                if (ToCanvasX(c.wy, c.wx, c.left) < 2) {
                    c.left--;
                }
                c.wx--;
                r = true;
            } else {
                GoUp(c, d) && GoEnd(c, d);
                r = true;
            }
        }
        return r;
    }
    //
    bool Controller::GoUp(Cursol& c, const Document& d, int l) {
        bool r = false;
        for (int ii = 0; ii < l; ii++) {
            if (c.wy > 1) {
                int cx = ToCanvasX(c.wy, c.wx, c.left);
                if (ToCanvasY(c.wy, c.top) == 1) {
                    c.top--;
                }
                c.wy--;
                if (cx != ToCanvasX(c.wy, c.wx, c.left)) {
                    c.wx = ToWorldX(c.wy, cx, c.left);
                    if (c.left > c.wx) {
                        c.left = c.wx;
                    }
                }
                r = true;
            }
        }
        return r;
    }
    //
    bool Controller::GoDown(Cursol& c, const Document& d, int l) {
        bool r = false;
        for (int ii = 0; ii < l; ii++) {
            if (c.wy < d.GetMaxLineNumber()) {
                int cx = ToCanvasX(c.wy, c.wx, c.left);
                if (ToCanvasY(c.wy, c.top) > Canvas::MAX_CANVASY - 2) {
                    c.top++;
                }
                c.wy++;
                if (cx != ToCanvasX(c.wy, c.wx, c.left)) {
                    c.wx = ToWorldX(c.wy, cx, c.left);
                    if (c.left > c.wx) {
                        c.left = c.wx;
                    }
                }
                r = true;
            } else if (c.wy == d.GetMaxLineNumber() && d.GetLine(c.wy)->endsWith("\n")) {
                if (ToCanvasY(c.wy, c.top) > Canvas::MAX_CANVASY - 2) {
                    c.top++;
                }
                c.wy++;
                c.wx = c.left = 1;
                r = true;
            }
        }
        return r;
    }
    //
    bool Controller::Copy(Cursol& c, const Document& d, Document& cl) {
        bool r = false;
        if (c.range) {
            cl = d.GetSubDocument(c.ry, c.rx, c.wy, c.wx);
            c.range = false;
            r = true;
        }
        return r;
    }
    //
    bool Controller::Cut(Cursol& c, Document& d, Document& cl) {
        bool r = false;
        if (c.range) {
            cl = d.GetSubDocument(c.ry, c.rx, c.wy, c.wx);
            Delete(c, d);
        }
        return r;
    }
    //
    bool Controller::Paste(Cursol& c, Document& d, const Document& cl) {
        return cl.GetMaxLineNumber() > 0 && d.Insert(cl, c.wy, c.wx) && GoRight(c, d, cl.GetLength());
    }
    //
    bool Controller::Delete(Cursol& c, Document& d) {
        bool r = false;
        if (c.range) {
            d.Delete(c.ry, c.rx, c.wy, c.wx);
            if (c.ry < c.wy) {
                Go(c, d, c.ry, c.rx);
            }
            c.range = false;
            r = true;
        } else {
            r = d.Delete(c.wy, c.wx);
        }
        return r;
    }
    //
    bool Controller::BackSpace(Cursol& c, Document& d) {
        bool r = false;
        if (GoLeft(c, d)) {
            d.Delete(c.wy, c.wx);
            r = true;
        }
        return r;
    }
    //
    bool Controller::PageDown(Cursol& c, const Document& d) {
        bool r = false;
        if (c.wy + Canvas::MAX_CANVASY <= d.GetMaxLineNumber()) {
            int cx = ToCanvasX(c.wy, c.wx, c.left);
            c.top += Canvas::MAX_CANVASY;
            c.wy += Canvas::MAX_CANVASY;
            if (cx != ToCanvasX(c.wy, c.wx, c.left)) {
                c.wx = ToWorldX(c.wy, cx, c.left);
                if (c.left > c.wx) {
                    c.left = c.wx;
                }
            }
            r = true;
        }
        return r;
    }
    //
    bool Controller::PageUp(Cursol& c, const Document& d) {
        bool r = false;
        if (c.wy - Canvas::MAX_CANVASY > 0) {
            int cx = ToCanvasX(c.wy, c.wx, c.left);
            c.top = (c.top - Canvas::MAX_CANVASY > 0) ? c.top - Canvas::MAX_CANVASY : 1;
            c.wy -= Canvas::MAX_CANVASY;
            if (cx != ToCanvasX(c.wy, c.wx, c.left)) {
                c.wx = ToWorldX(c.wy, cx, c.left);
                if (c.left > c.wx) {
                    c.left = c.wx;
                }
            }
            r = true;
        }
        return r;
    }
    //
    bool Controller::Go(Cursol& c, const Document& d, int y, int x) {
        //
        if (y < 1 || d.GetMaxLineNumber() < 1) {
            y = 1;
        } else {
            String* s = d.GetLine(d.GetMaxLineNumber());
            if (s->endsWith("\n") && y > d.GetMaxLineNumber() + 1) {
                y = d.GetMaxLineNumber() + 1;
            } else if (!s->endsWith("\n") && y > d.GetMaxLineNumber()) {
                y = d.GetMaxLineNumber();
            }
        }
        String* s = d.GetLine(y);
        if (x < 1 || !s) {
            x = 1;
        } else {
            if (s->endsWith("\n") && x > s->length()) {
                x = s->length();
            } else if (!s->endsWith("\n") && x > s->length() + 1) {
                x = s->length() + 1;
            }
        }
        //
        if (y < c.top || y >= c.top + Canvas::MAX_CANVASY) {
            if (y < Canvas::MAX_CANVASY) {
                c.top = 1;
            } else {
                c.top = y - (Canvas::MAX_CANVASY - 2);
            }
        }
        int cl = ToCanvasString(d, y).length();
        if (cl < c.left || cl > c.left + (Canvas::MAX_CANVASX - 3)) {
            if (cl < Canvas::MAX_CANVASX) {
                c.left = 1;
            } else {
                c.left = cl - (Canvas::MAX_CANVASX - 3);
            }
        }
        c.wx = x;
        c.wy = y;
        return true;
    }
    //
    void Controller::SetCursolRange(Cursol& c, int modifiers) {
        if (modifiers == KeyEvent::VKEY_RSHIFT || modifiers == KeyEvent::VKEY_LSHIFT) {
            if (!c.range) {
                c.range = true;
                c.rx = c.wx;
                c.ry = c.wy;
            }
        } else if (c.range) {
            c.range = false;
        }
    }
    //
    bool Controller::New(Cursol& c, Document& d) {
        document.Clear();
        cursol.range = false;
        cursol.left = cursol.top = cursol.wx = cursol.wy = 1;
        return true;
    }
    //
    bool Controller::WriteFile(const String& f, const Document& d) {
        // getBytes() is utf8, anway currently only ascii support for input.
        // so this is enough for other application.
        FILE *fp = fopen(f.getBytes(), "w");
        if(fp == NULL)
        {
            // how to show message?
            fprintf(stderr, "file: %s can't opened\n", f.getBytes());
            return false;
        }
        int max = d.GetMaxLineNumber();
        int i;
        for(i = 1; i <= max; i++)
        {
            String* line = d.GetLine(i);
            fwrite(line->getBytes(), 1, line->length(), fp);
        }
        fclose(fp);
        return true;
    }
    //
    bool Controller::ReadFile(const String& f, Document& d) {
        monapi_cmemoryinfo* mi = monapi_file_read_all(f.getBytes());
        if (!mi || mi->Size <= 0) {
            return false;
        }
        for (unsigned int ii = 0; ii < mi->Size; ii++) {
            if (mi->Data[ii] == '\r') {
                continue;
            }
            // tooo bad :-)
            d.Append(mi->Data[ii]);
        }
        return true;
    }
    //
    void Controller::Exit() {
        window.stop();
    }
    //
    void Controller::Run() {
        //
        if (file.length()) {
            window.setTitle(file + " - " + window.getTitle());
            ReadFile(file, document);
        }
        //
        window.SetEventListener(this);
        window.SetDocument(&document);
        window.GetCanvas()->SetCursol(&cursol);
        // 12? 28? :-)
        window.setBounds(10, 10, window.GetCanvas()->getWidth() + 12, window.GetCanvas()->getHeight() + 20 + 28);
        window.setTimer(TIMER_INTERVAL);
        window.run();
    }
}
