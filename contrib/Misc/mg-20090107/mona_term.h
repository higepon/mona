/*
 * mona_term.h - 
 *
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
 *
 *  $Id: mona_term.h 261 2008-07-25 06:16:44Z higepon $
 */

#ifndef _MONA_TERM_
#define _MONA_TERM_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

    void mona_ttmove(int row, int col);
    void mona_tteeol();
    void mona_tteeop();
    void mona_ttbeep();
    void mona_ttinsl(int row, int bot, int nchunk);
    void mona_ttdell(int row, int bot, int nchunk);
    void mona_ttwindow(int top, int bot);
    void mona_ttnowindow();
    void mona_ttcolor(int color);
    int mona_ttwait(int msec);
    int mona_charswaiting();
    int mona_ttgetc();
    void mona_ttflush();
    int mona_ttputc(int c);
    int mona_ttcooked();
    void mona_ttclose();
    void mona_ttopen();
    int mona_ttraw();
    void bcopy(const void* src, void* dest, size_t n);
    void bzero(void* to, size_t count);
    extern int ttcol;
    extern int ttrow;
    extern int ncol;
    extern int nrow;

#ifdef __cplusplus
}
#endif

#endif // _MONA_TERM_
