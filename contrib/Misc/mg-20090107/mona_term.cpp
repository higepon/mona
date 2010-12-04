#include "mona_term.h"
#include <monapi.h>

int ncol;
int nrow;

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
}
void mona_tteeol()
{
}
void mona_tteeop()
{
}
void mona_ttbeep()
{
}
void mona_ttinsl(int row, int bot, int nchunk)
{
}
void mona_ttdell(int row, int bot, int nchunk)
{
}
void mona_ttwindow(int top, int bot)
{
}
void mona_ttnowindow()
{
}
void mona_ttcolor(int color)
{
}
int mona_ttwait(int msec)
{
}
int mona_charswaiting()
{
}
int mona_ttgetc()
{
}
void mona_ttflush()
{
}
int mona_ttputc(int c)
{
}
int mona_ttcooked()
{
}
void mona_ttclose()
{
}
void mona_ttopen()
{
}
int mona_ttraw()
{
}

void mona_ttresize()
{
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
