#include "mona_term.h"
#include <monapi.h>

int ttcol;
int ttrow;
int ncol;
int nrow;

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
