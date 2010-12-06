#include "mona_term.h"
#include "chrdef.h"
#include <monapi.h>
#include <baygui.h>
#include <string>

using namespace MonAPI;

int ncol;
int nrow;

class MgFrame : public Frame {
private:
    enum {
        MODE_LINE_COLS = 2,
        MAX_NUM_ROWS = 24 + MODE_LINE_COLS,
        MAX_NUM_COLS = 80
    };
    uint32_t parentTid_;
    std::string lines[MAX_NUM_ROWS];
    uint16_t currentRow_;
    uint16_t currentCol_;

    int fontWidth()
    {
        return getFontMetrics()->getWidth("W");
    }

    int fontHeight()
    {
        return getFontMetrics()->getHeight("W");
    }

public:
    MgFrame(uint32_t parentTid) : parentTid_(parentTid), currentRow_(0), currentCol_(0)
    {
        setTitle("mg");
        setBounds(450, 40, MAX_NUM_COLS * fontWidth(), MAX_NUM_ROWS * fontHeight());
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
        }
    }

    void moveCursor(uint16_t col, uint16_t row)
    {
        if (col >= MAX_NUM_COLS) {
            _logprintf("col=%d\n", col);

        }
        ASSERT(row < MAX_NUM_ROWS);
        ASSERT(col < MAX_NUM_COLS);
        currentRow_ = row;
        currentCol_ = col;
    }

    void putc(int c)
    {
        _logprintf("%s %s:%d<%c>\n", __func__, __FILE__, __LINE__, c);
        ASSERT(currentRow_ < MAX_NUM_ROWS);
        ASSERT(currentCol_ < MAX_NUM_COLS);
        std::string& line = lines[currentRow_];
        if (line.size() == currentCol_) {
            line += c;
        } else if (line.size() < currentCol_) {
            for (uint16_t i = 0; i < currentCol_ - line.size() + 1; i++) {
                line += ' ';
            }
            line += c;
        } else {
            line.erase(line.begin() + currentCol_, line.end());
            line += c;
        }
        currentCol_++;
    }

    virtual void paint(Graphics* g)
    {
        g->setColor(getForeground());
        for (int i = 0; i < MAX_NUM_ROWS; i++) {
            std::string& line = lines[i];
            if (line.empty()) {
                continue;
            }
            g->setColor(getBackground());
            g->fillRect(0, i * fontHeight(), MAX_NUM_COLS * fontWidth(), fontHeight());
            g->setColor(getForeground());
            _logprintf("line:<%s>\n", line.c_str());
            g->drawString(line.c_str(), 0, fontHeight() * i);
        }
  }

        // if (event->getType() == Event::CUSTOM_EVENT) {
        //     if (event->header == MSG_TEXT) {
        //         size_t length = MESSAGE_INFO_MAX_STR_LENGTH < event->arg1 ? MESSAGE_INFO_MAX_STR_LENGTH : event->arg1;
        //         string text(event->str, length);
        //         string content(textArea_->getText());
        //         content += text;printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        //         textArea_->setText(content.c_str());
        //         repaint();
        //     }
        // }
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
    _logprintf("vasprintf"); // tekito
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
    _logprintf("chdir to <%s>\n", path);
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
    g_frame->moveCursor(col, row);
    logprintf("%s %s:%d row=%d col=%d\n", __func__, __FILE__, __LINE__, row, col);
}
void mona_tteeol()
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}
void mona_tteeop()
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}
void mona_ttbeep()
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}
void mona_ttinsl(int row, int bot, int nchunk)
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}
void mona_ttdell(int row, int bot, int nchunk)
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}
void mona_ttwindow(int top, int bot)
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}
void mona_ttnowindow()
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}
void mona_ttcolor(int color)
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}
int mona_ttwait(int msec)
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}
int mona_charswaiting()
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
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
    MessageInfo info;
    while(M_OK == MonAPI::Message::receive(&info)) {
        if(info.header == MSG_KEY_VIRTUAL_CODE) {
            int keycode  = info.arg1;
            int modifiers = info.arg2;
            int mod = 0;
            logprintf("keycode=%x", keycode);
            if (modifiers & KeyEvent::VKEY_CTRL) {
                logprintf("ctrl!!!");
                return keycode - 'a' + 1; // Ctrl-A = 1, Ctrl-Z = 26
            }
            if (keycode == KeyEvent::VKEY_ENTER) {
                _logprintf("ENTER");
                return 0x0d | mod;
            } else {
                return keycode | mod;
            }
        }
    }
    return '\n';
}
void mona_ttflush()
{
    ASSERT(g_frame);
    // todo message でやるほうが行儀が良い
    g_frame->repaint();
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}
int mona_ttputc(int c)
{
    #ifdef MONA
    // todo ttputc と同様にバッファリングすべき
    #endif
    ASSERT(g_frame);
    g_frame->putc(c);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}
int mona_ttcooked()
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}
void mona_ttclose()
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}
void mona_ttopen()
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}
int mona_ttraw()
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}

void mona_ttresize()
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
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
