/* implement later */

extern "C" {

#include "fm.h"

int LINES, COLS;
typedef unsigned short l_prop;

#define S_SCREENPROP    0x0f
#define S_NORMAL        0x00
#define S_STANDOUT      0x01
#define S_UNDERLINE     0x02
#define S_BOLD          0x04
#define S_EOL           0x08

/* Line status */
#define L_DIRTY         0x01
#define L_UNUSED        0x02
#define L_NEED_CE       0x04
#define L_CLRTOEOL      0x08



typedef struct scline {
#ifdef USE_M17N
    char **lineimage;
#else
    char *lineimage;
#endif
    l_prop *lineprop;
    short isdirty;
    short eol;
} Screen;

static Screen *ScreenElem = NULL, **ScreenImage = NULL;

static int CurLine, CurColumn;

static int max_LINES = 0, max_COLS = 0;

void
setupscreen(void)
{
    int i;
    if (LINES + 1 > max_LINES) {
	max_LINES = LINES + 1;
	max_COLS = 0;
	ScreenElem = New_N(Screen, max_LINES);
	ScreenImage = New_N(Screen *, max_LINES);
    }
    if (COLS + 1 > max_COLS) {
	max_COLS = COLS + 1;
	for (i = 0; i < max_LINES; i++) {
#ifdef USE_M17N
	    ScreenElem[i].lineimage = New_N(char *, max_COLS);
	    bzero((void *)ScreenElem[i].lineimage, max_COLS * sizeof(char *));
#else
	    ScreenElem[i].lineimage = New_N(char, max_COLS);
#endif
	    ScreenElem[i].lineprop = New_N(l_prop, max_COLS);
	}
    }
    for (i = 0; i < LINES; i++) {
	ScreenImage[i] = &ScreenElem[i];
	ScreenImage[i]->lineprop[0] = S_EOL;
	ScreenImage[i]->isdirty = 0;
    }
    for (; i < max_LINES; i++) {
	ScreenElem[i].isdirty = L_UNUSED;
    }
}


void
move(int line, int column)
{
    if (line >= 0 && line < LINES)
	CurLine = line;
    if (column >= 0 && column < COLS)
	CurColumn = column;
}

void
clrtoeolx(void)
{
}
void
addstr(char *s)
{
}

void
standout(void)
{
}

void
addch(char c)
{
  int i;
  char *p;
  
  p = ScreenImage[CurLine]->lineimage;
  i = CurColumn;

  if(c == '\n') {
    wrap();
  } else {
    p[i] = c;
  }
}

void
wrap(void)
{
    CurLine++;
    CurColumn = 0;
}


void
refresh(void)
{
    
}

void
term_raw(void)
{
}


void
term_cbreak(void)
{
}

void
addnstr(char *s, int n)
{
}

void
clrtobotx(void)
{
}

void
setfcolor(int color)
{
}

void
setbcolor(int color)
{
}

void
standend(void)
{
}

void
addnstr_sup(char *s, int n)
{
}

void
toggle_stand(void)
{
}

char
getch(void)
{
   return 'a';
}

void
reset_tty(void)
{
}

void
mouse_end()
{
}

int
initscr(void)
{
    return 0; /* 0 seems success */
}

void
term_noecho(void)
{
}

int
graph_ok(void)
{
    return 0;
}

void
graphstart(void)
{
}

void
graphend(void)
{
}


void
underline(void)
{
}

void
underlineend(void)
{
}

void
bold(void)
{
}

void
boldend(void)
{
}

void
term_title(char *s)
{
}

int
sleep_till_anykey(int sec, int purge)
{
    return 0;
}

void
mouse_active()
{
}

void
mouse_inactive()
{
}

void
bell(void)
{
}

/* extern "C" */ }

#undef init
#include "mona_w3m.h"

extern W3MFrame *g_frame;


