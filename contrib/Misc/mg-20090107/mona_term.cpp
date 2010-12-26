#include "mona_term.h"
#include "chrdef.h"
#include <monapi.h>
#include <baygui.h>
#include <string>
extern "C" {
#include "def.h"
}

#if 1
#define mona_trace(...) /* */
#else
// #define mona_trace(...) _logprintf(__VA_ARGS__)
#endif

using namespace MonAPI;

int ncol;
int nrow;

class MgFrame : public Frame {
public:
    enum {
        MODE_LINE_COLS = 2,
        MAX_NUM_ROWS = MG_NUM_ROWS + MODE_LINE_COLS,
        MAX_NUM_COLS = MG_NUM_COLS,
        TIMER_INTERVAL = 500
    };
private:
    uint32_t parentTid_;
    std::vector<std::string> lines;
    bool colors[MAX_NUM_ROWS];
    uint16_t currentRow_;
    uint16_t currentCol_;
    bool cursorEnabled_;

    int fontWidth(int ch)
    {
        char buf[2];
        buf[0] = ch;
        buf[1] = 0;
        int style = getFontMetrics()->getFontStyle();
        getFontMetrics()->setFontStyle(Font::FIXED);
        int ret = getFontMetrics()->getWidth(buf);
        getFontMetrics()->setFontStyle(style);
        return ret;

    }

    int fontWidth()
    {
        return fontWidth('W');
    }

    int fontHeight()
    {
        int style = getFontMetrics()->getFontStyle();
        getFontMetrics()->setFontStyle(Font::FIXED);
        int ret = getFontMetrics()->getHeight("W");
        getFontMetrics()->setFontStyle(style);
        return ret;
    }

public:
    MgFrame(uint32_t parentTid) : parentTid_(parentTid), currentRow_(0), currentCol_(0), cursorEnabled_(false)
    {
        for (int i = 0; i < MAX_NUM_ROWS; i++) {
            lines.push_back("");
        }
        std::fill(colors, colors + MAX_NUM_ROWS, false);
        setTitle("mg");
        setBounds(450, 40, MAX_NUM_COLS * fontWidth(), MAX_NUM_ROWS * fontHeight() + 4);
        setForeground(baygui::Color::white);
        setForeground(baygui::Color::black);
        setTimer(TIMER_INTERVAL);
    }

    ~MgFrame()
    {
    }

    void processEvent(Event* event)
    {
        if (event->getType() == KeyEvent::KEY_PRESSED) {
            int keycode = ((KeyEvent *)event)->getKeycode();
            int modifiers = ((KeyEvent*)event)->getModifiers();
            if (Message::send(parentTid_, MSG_KEY_VIRTUAL_CODE, keycode, modifiers) != M_OK) {
                monapi_fatal("parent is not exist");
            }
        } else if (event->getType() == Event::TIMER) {
            cursorEnabled_ = !cursorEnabled_;
            repaint();
            setTimer(TIMER_INTERVAL);
        } else if (event->getType() == Event::CUSTOM_EVENT) {
            if (event->header == MSG_STOP) {
                stop();
            }
        }

    }

    void setColor(bool reversed)
    {
        colors[currentRow_] = reversed;
    }

    void moveCursor(uint16_t col, uint16_t row)
    {
        if (col >= MAX_NUM_COLS) {
            mona_trace("col=%d\n", col);
        }
        ASSERT(row < MAX_NUM_ROWS);
        ASSERT(col < MAX_NUM_COLS);
        currentRow_ = row;
        std::string& line = lines[row];
        size_t lineSize = line.size();
        if (col > lineSize) {
            for (size_t i = 0; i < col - lineSize + 1; i++) {
                line += ' ';
            }
        }
        currentCol_ = col;
    }

    void eraseToEndOfPage()
    {
        std::string& head = lines[currentRow_];
        head.erase(head.begin() + currentCol_, head.end());
        for (uint16_t i = currentRow_ + 1; i < MAX_NUM_ROWS; i++) {
            lines[i].clear();
        }
    }

    void eraseToEndOfLine()
    {
        std::string& line = lines[currentRow_];
        line.erase(line.begin() + currentCol_, line.end());
    }

    // void clearLines(int startRow, int endRow)
    // {
    //     for (uint16_t i = startRow; i <= endRow; i++) {
    //         lines[i].clear();
    //     }
    // }

    void deleteLinesShiftUp(int numLines)
    {
        // delete current n lines and shift up
        lines.erase(lines.begin() + currentRow_, lines.begin() + currentRow_ + numLines);

        // empty lines are added to the bottom.
        for (int i = 0; i < numLines; i++) {
            lines.push_back("");
        }
    }

    void insertLinesShiftDown(int numLines)
    {
        for (int i = 0; i < numLines; i++) {
            lines.insert(lines.begin() + currentRow_, "");
        }

        lines.erase(lines.end() - currentRow_, lines.end());
    }


    void putc(int c)
    {
        ASSERT(currentRow_ < MAX_NUM_ROWS);
        ASSERT(currentCol_ < MAX_NUM_COLS);
        if (c == 0x0d || c == '\t') {
            return; // is this right?
        }
        std::string& line = lines[currentRow_];
        if (c == '\b') {
            ASSERT(currentRow_ != 0);
            line.erase(line.begin() + currentCol_, line.begin() + currentCol_);
            currentCol_--;
            return;
        }
        if (line.size() == currentCol_) {
            mona_trace("line += %x\n", c);
            line += c;
        } else if (line.size() < currentCol_) {
            assert(0);
            size_t lineSize = line.size();
            for (size_t i = 0; i < currentCol_ - lineSize + 1; i++) {
                line += ' ';
            }
            mona_trace("line2 += %x currentCol_ - lineSize + 1=%d\n", c, currentCol_ - lineSize + 1);
            line += c;
        } else {
//            line.erase(line.begin() + currentCol_, line.end());
            mona_trace("line3 += %x\n", c);
            line[currentCol_] = c;
        }
        currentCol_++;
    }

    virtual void paint(Graphics* g)
    {
        g->setFontStyle(Font::FIXED);
        for (int i = 0; i < MAX_NUM_ROWS; i++) {
            std::string& line = lines[i];
            g->setColor(colors[i] ? baygui::Color::white : baygui::Color::black);
            g->fillRect(0, i * fontHeight(), MAX_NUM_COLS * fontWidth(), fontHeight());
            if (line.empty()) {
                continue;
            }
            g->setColor(colors[i] ? baygui::Color::black : baygui::Color::white);
            g->drawString(line.c_str(), 0, fontHeight() * i);
            mona_trace("line<%s>\n", line.c_str());
        }
        if (cursorEnabled_) {
            g->setColor(baygui::Color::white);
            int leftOffset = 0;
            std::string& line = lines[currentRow_];
            for (int i = 0; i < currentCol_; i++) {
                leftOffset += fontWidth(line[i]);
            }
            int cursorWidth = fontWidth();
            if (line.size() > currentCol_) {
                cursorWidth = fontWidth(line[currentCol_]);
            }
            g->fillRect(leftOffset, currentRow_ * fontHeight(), cursorWidth, fontHeight());
        }
  }
};

MgFrame* g_frame;

// wrapperPlayLoop for thread
static void __fastcall frameThread(void* arg)
{
    uint32_t parent = (uint32_t)arg;
    g_frame = new MgFrame(parent);
    if (Message::send(parent, MSG_STARTED) != M_OK) {
        monapi_fatal("parent not found");
    }
    g_frame->run();
    delete g_frame;
}

void mona_frame_stop(uint32_t tid)
{
    if (Message::send(tid, MSG_STOP) != M_OK) {
        monapi_fatal("frame stop failed");
    }
}

uint32_t mona_frame_init()
{
    uint32_t parentTid = System::getThreadID();
    uint32_t tid = mthread_create_with_arg(frameThread, (void*)parentTid);
    return tid;
}

void mona_frame_wait_startup(uint32_t tid)
{
    MessageInfo info;
    while(M_OK == Message::receive(&info)) {
        if(info.header == MSG_STARTED && info.from == tid) {
            break;
        }
    }
}


int vasprintf(char **strp, const char *fmt, va_list ap)
{
    const int BUFFER_SIZE = 1024 * 10;
    *strp = (char*)malloc(BUFFER_SIZE);
    if (*strp == NULL) {
        return -1;
    }
    return vsprintf(*strp, fmt, ap);
}

char* getcwd(char* buf, size_t size)
{
    if (size > 1) {
        strcpy(buf, "/");
        return buf;
    } else {
        return NULL;
    }
}

int chdir(const char* path)
{
    mona_trace("chdir to <%s>\n", path);
    return 0;
}


void bcopy(const void* src, void* dest, size_t n)
{
    memcpy(dest, src, n);
}

void bzero(void* to, size_t count)
{
    memset (to, 0, count);
}


void mona_ttmove(int row, int col)
{
    mona_trace("row=%d col=%d %s %s:%d\n", row, col, __func__, __FILE__, __LINE__);
    g_frame->moveCursor(col, row);
    ttrow = row;
    ttcol = col;
}

void mona_tteeol()
{
    mona_trace("%s %s:%d\n", __func__, __FILE__, __LINE__);
    g_frame->eraseToEndOfLine();
    // TODo
    // this causes crash
    // ttrow = HUGE;
    // ttcol = HUGE;
}

void mona_tteeop()
{
    mona_trace("%s %s:%d\n", __func__, __FILE__, __LINE__);
    g_frame->eraseToEndOfPage();
    ttrow = ttcol = HUGE;
    mona_trace("%s %s:%d\n", __func__, __FILE__, __LINE__);
}
void mona_ttbeep()
{
    mona_trace("%s %s:%d\n", __func__, __FILE__, __LINE__);
}
void mona_ttinsl(int row, int bot, int nchunk)
{
    int i, nl;
    mona_trace("%s %s:%d\n", __func__, __FILE__, __LINE__);
    /* Case of one line insert is special. */
    if (row == bot) {
        ttmove(row, 0);
        tteeol();
        return;
    }
    ttmove(1 + bot - nchunk, 0);
    nl = nrow - ttrow;
    /* For all lines in the chunk... */
    for (i = 0; i < nchunk; i++) {
        g_frame->deleteLinesShiftUp(nl);
    }
    ttmove(row, 0);

    /* ttmove() changes ttrow */
    nl = nrow - ttrow;

    /* For all lines in the chunk */
    for (i = 0; i < nchunk; i++) {
        g_frame->insertLinesShiftDown(nl);
    }
    ttrow = HUGE;
    ttcol = HUGE;
}

// putpad(str, num) : alias for tputs(str, num, ttputc)
//   str : padding information such as delete_line or insert_line.
//   num : number of lines affected
//
// padding information : http://chaos4.phy.ohiou.edu/~thomas/ref/info/termcap/Insdel_Line.html
//   delete_line: String of commands to delete the line the cursor is on. The following lines move up, and a blank line appears at the bottom of the screen (or bottom of the scroll region). If the terminal has the `db' flag, a nonblank line previously pushed off the screen bottom may reappear at the bottom.
//   The cursor must be at the left margin before this command is used. This command does not move the cursor.

void mona_ttdell(int row, int bot, int nchunk)
{
    mona_trace("%s %s:%d\n", __func__, __FILE__, __LINE__);
    ttmove(row, 0);
    int nl = MgFrame::MAX_NUM_ROWS - ttrow;

    /* For all lines in the chunk    */
    for (int i = 0; i < nchunk; i++) {
        g_frame->deleteLinesShiftUp(nl);
    }

    ttmove(1 + bot - nchunk, 0);

    for (int i = 0; i < nchunk; i++) {
        g_frame->insertLinesShiftDown(nl);
    }
    ttrow = HUGE;
    ttcol = HUGE;
}
void mona_ttwindow(int top, int bot)
{
    mona_trace("%s %s:%d\n", __func__, __FILE__, __LINE__);
}
void mona_ttnowindow()
{
    mona_trace("%s %s:%d\n", __func__, __FILE__, __LINE__);
}

void mona_ttcolor(int color)
{
    ASSERT(color == 0 || color == 1 || color == 2);
    if (color == 0 || color == 1) {
        // do nothing
    } else {
        g_frame->setColor(true);
    }
}

// OK
int mona_ttwait(int msec)
{
    uint32_t timerId = set_timer(msec);
    MessageInfo msg;

    for (int i = 0; ; i++) {
        int result = MonAPI::Message::peek(&msg, i);

        if (result != M_OK) {
            i--;
            syscall_mthread_yield_message();
        } else if (msg.header == MSG_TIMER) {
            if (msg.arg1 != timerId) {
                continue;
            }
            kill_timer(timerId);
            if (MonAPI::Message::peek(&msg, i, PEEK_REMOVE) != M_OK) {
                monapi_fatal("peek error %s:%d\n", __FILE__, __LINE__);
            }
            return (FALSE);
        } else if (msg.header == MSG_KEY_VIRTUAL_CODE) {
            kill_timer(timerId);
            return (TRUE);
        }
    }
}

// OK
int mona_charswaiting()
{
    return 0;
}

// only support often-used char for a while
static inline char keyToChar(int keycode, int modifiers, int charcode)
{
  if(modifiers & KEY_MODIFIER_CTRL) {
    int basecode = keycode-'A'+1;
    if(basecode >= 0 && basecode <= 32)
      return basecode;
    return -1;
  }
  if(charcode)
    return charcode;
  switch(keycode) {
  case MonAPI::Keys::Enter:
    return 0x0d;
  case MonAPI::Keys::Right:
    return 6;
  case MonAPI::Keys::Left:
    return 2;
  // case MonAPI::Keys::Tab:
  case MonAPI::Keys::Back:
    return MonAPI::Keys::Back;
  case MonAPI::Keys::Delete:
    return  127;
  }
  return -1;
}

int mona_ttgetc()
{
    static bool hasUngetc = false;
    static int ungetc;
    if (hasUngetc) {
        hasUngetc = false;
        return ungetc;
    }
    MessageInfo info;
    while(M_OK == MonAPI::Message::receive(&info)) {
        if(info.header != MSG_KEY_VIRTUAL_CODE) {
            continue;
        }
        int keycode  = info.arg1;
        int modifiers = info.arg2;
        if (modifiers == KeyEvent::VKEY_CTRL) {
            if ('a' <= keycode && keycode <= 'z') {
                return keycode - 'a' + 1; // Ctrl-A = 1, Ctrl-Z = 26
            } else if (keycode == 0x20) {
                return 0;
            } else if (keycode == '/') {
                return 0x1f;
            } else {
                assert(!hasUngetc);
                hasUngetc = true;
                ungetc = keycode;
                return 0x18;
            }

        } else if (modifiers == KeyEvent::VKEY_ALT) {
            assert(!hasUngetc);
            hasUngetc = true;
            ungetc = keycode;
            return '\e';
        }
        mona_trace("mona_ttgetc=%x\n", keycode);
        switch (keycode) {
        case KeyEvent::VKEY_ENTER:
            return 0x0d;
        case KeyEvent::VKEY_TAB:
            return '\t';
        case KeyEvent::VKEY_BACKSPACE:
            return 0x7f;
        // case KeyEvent::VKEY_ALT:
        // case KeyEvent::VKEY_LMENU:
        //     return '\e';
        default:
            return keycode;
        }
    }
    return '\n';
}

// OK
void mona_ttflush()
{
    ASSERT(g_frame);
    // todo message でやるほうが行儀が良い
    g_frame->repaint();
}

// OK
int mona_ttputc(int c)
{
    mona_trace("putc<%c>\n", c);
    #ifdef MONA
    // todo ttputc と同様にバッファリングすべき
    #endif
    ASSERT(g_frame);
    g_frame->putc(c);
}

// OK
int mona_ttcooked()
{
    mona_trace("%s %s:%d\n", __func__, __FILE__, __LINE__);
}

// OK
void mona_ttclose()
{
    mona_trace("%s %s:%d\n", __func__, __FILE__, __LINE__);
}

// OK
void mona_ttopen()
{
    /* do nothing */
}

int mona_ttraw()
{
    mona_trace("%s %s:%d\n", __func__, __FILE__, __LINE__);
    assert(0);
}

// OK
void mona_ttresize()
{
    mona_trace("%s %s:%d\n", __func__, __FILE__, __LINE__);
}

void mona_get_file_datetime_size(const char* file, int* year, int* month, int* day, int* hour, int* min, int* sec, int* size)
{
    MonAPI::Date date;
    intptr_t ret = monapi_file_get_date(file, date);
    ASSERT(ret == M_OK);
    *year = date.year();
    *month = date.month();
    *day = date.day();
    *hour = date.hour();
    *min = date.min();
    *sec = date.sec();
    uint32_t s;
    ret = monapi_file_get_file_size_by_path(file, s);
    ASSERT(ret == M_OK);
    *size = s;
}

/*      $OpenBSD: strsep.c,v 1.6 2005/08/08 08:05:37 espie Exp $        */

/*-
 * Copyright (c) 1990, 1993
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <string.h>
#include <stdio.h>

/*
 * Get next token from string *stringp, where tokens are possibly-empty
 * strings separated by characters from delim.
 *
 * Writes NULs into the string at *stringp to end tokens.
 * delim need not remain constant from call to call.
 * On return, *stringp points past the last NUL written (if there might
 * be further tokens), or is NULL (if there are definitely no more tokens).
 *
 * If *stringp is NULL, strsep returns NULL.
 */
char *
strsep(char **stringp, const char *delim)
{
        char *s;
        const char *spanp;
        int c, sc;
        char *tok;

        if ((s = *stringp) == NULL)
                return (NULL);
        for (tok = s;;) {
                c = *s++;
                spanp = delim;
                do {
                        if ((sc = *spanp++) == c) {
                                if (c == 0)
                                        s = NULL;
                                else
                                        s[-1] = 0;
                                *stringp = s;
                                return (tok);
                        }
                } while (sc != 0);
        }
        /* NOTREACHED */
}
