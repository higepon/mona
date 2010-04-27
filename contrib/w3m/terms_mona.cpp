extern "C" {
#include "fm.h"
}

#undef init
#include "mona_w3m.h"

extern W3MFrame *g_frame;


extern "C" {

int LINES, COLS;


static l_prop CurrentMode = 0;


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
      {
	CurLine = line;
      }
    if (column >= 0 && column < COLS)
	CurColumn = column;
}

void
clrtoeolx(void)
{
  int i;
  char *p;
  p = ScreenImage[CurLine]->lineimage;
  for (i = CurColumn; i < COLS; i++)
    p[i] = ' ';
}

void
addstr(char *s)
{
    while (*s != '\0')
	addch(*(s++));
}

void
standout(void)
{
}

void
addch(char c)
{
/*
static int count = 0;
  if(count > 30) {
    MONA_TRACE(" )\naddch(");
    count = 0;
  }
MONA_TRACE_FMT((stderr, " %c ", c)); count++;
*/

  int i;
  char *p;
  l_prop *pr;

  
  p = ScreenImage[CurLine]->lineimage;
  pr = ScreenImage[CurLine]->lineprop;
  
  i = CurColumn;

  if(c == '\n' || c == '\r') {
    wrap();
  } else {
    if(CurColumn >= max_COLS) {
      wrap();
      return; // do nothing
    }
    p[i] = c;
    pr[i] = CurrentMode;
    ScreenImage[CurLine]->isdirty |= L_DIRTY;
    CurColumn++;
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
MONA_TRACE("refresh1\n");
    if(g_frame)
    {
MONA_TRACE("refresh2\n");
      g_frame->setScreenImage(ScreenImage);
      // g_frame->repaint();
      g_frame->updatePane();
    }
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
    int i;
    for (i = 0; i < n && *s != '\0'; i++)
	addch(*(s++));
}

void
clrtobot_eol(void (*clrtoeol) ())
{
    int l, c;

    l = CurLine;
    c = CurColumn;
    (*clrtoeol) ();
    CurColumn = 0;
    CurLine++;
    for (; CurLine < LINES; CurLine++)
	(*clrtoeol) ();
    CurLine = l;
    CurColumn = c;
}


void
clrtobotx(void)
{
    clrtobot_eol(clrtoeolx);
}

void
setfcolor(int color)
{
    CurrentMode &= ~COL_FCOLOR;
    if ((color & 0xf) <= 7)
	CurrentMode |= (((color & 7) | 8) << 8);
}

void
setbcolor(int color)
{
    CurrentMode &= ~COL_BCOLOR;
    if ((color & 0xf) <= 7)
	CurrentMode |= (((color & 7) | 8) << 12);
}

void
standend(void)
{
}

void
addnstr_sup(char *s, int n)
{
  MONA_TRACE("addnstr_sup NYI\n");
}

void
toggle_stand(void)
{
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
    setupscreen();
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
  CurrentMode |= S_UNDERLINE;
}

void
underlineend(void)
{
    CurrentMode &= ~S_UNDERLINE;
}

void
bold(void)
{
    CurrentMode |= S_BOLD;
}

void
boldend(void)
{
    CurrentMode &= ~S_BOLD;
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

inline char
keyToChar(int keycode, int modcode, int charcode)
{
  if(charcode)
    return charcode;
  return keycode;
}

char
getch(void)
{
  MessageInfo info;
  while (0 == MonAPI::Message::receive(&info)) {
    if(info.header == MSG_KEY_VIRTUAL_CODE) {
      int keycode  = info.arg1;
      int modcode  = info.arg2;
      int charcode = info.arg3;
      // MONA_TRACE_FMT((stderr, "charcode=%x, %x, %x\n", charcode, keycode, modcode));
      if((modcode & KEY_MODIFIER_DOWN) == KEY_MODIFIER_DOWN) 
        return keyToChar(keycode, modcode, charcode);
    }
    // TODO: dispatch here.
  }
  return '\n';
}


/* extern "C" */ }




