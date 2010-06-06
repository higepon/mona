#pragma once

#include <baygui.h>

extern "C" {
#include <assert.h>

typedef unsigned short l_prop;
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

}

#define S_SCREENPROP    0x0f
#define S_NORMAL        0x00
#define S_STANDOUT      0x01
#define S_UNDERLINE     0x02
#define S_BOLD          0x04
#define S_EOL           0x08



#define COL_FCOLOR      0xf00
#define COL_FBLACK      0x800
#define COL_FRED        0x900
#define COL_FGREEN      0xa00
#define COL_FYELLOW     0xb00
#define COL_FBLUE       0xc00
#define COL_FMAGENTA    0xd00
#define COL_FCYAN       0xe00
#define COL_FWHITE      0xf00
#define COL_FTERM       0x000

#define COL_BCOLOR      0xf000
#define COL_BBLACK      0x8000
#define COL_BRED        0x9000
#define COL_BGREEN      0xa000
#define COL_BYELLOW     0xb000
#define COL_BBLUE       0xc000
#define COL_BMAGENTA    0xd000
#define COL_BCYAN       0xe000
#define COL_BWHITE      0xf000
#define COL_BTERM       0x0000

/* Line status */
#define L_DIRTY         0x01
#define L_UNUSED        0x02
#define L_NEED_CE       0x04
#define L_CLRTOEOL      0x08


#define C_WHICHCHAR     0xc0
#define C_ASCII         0x00
#define CHMODE(c)       ((c)&C_WHICHCHAR)
#define C_WCHAR1        0x40
#define C_WCHAR2        0x80

#define SPACE " "

#define S_DIRTY         0x20



extern Str wtf_to_utf8(char **pc, l_prop *pr, int len);


class W3MPane: public Component {
public:
  Screen **ScreenImage;
  static const int _xoffset = 10;
  static const int _yoffset = 10;

    int _defaultFG;
    int _defaultBG;
    bool _underLine;

  W3MPane() {
      _defaultFG = Color::black;
      _defaultBG = Color::white;
      setForeground(_defaultFG);
      setBackground(_defaultBG);
      _underLine = false;

    ScreenImage = NULL;
    setFontStyle(Font::FIXED);
  }

  inline int colWidth() {
    return getFontMetrics()->getWidth("W"); // tekito-
  }

  inline int colHeight() {
    return getFontMetrics()->getHeight("W"); // tekito-
  }

  TabBuffer *
    newTab(void)
      {
        TabBuffer *n;
        
        n = New(TabBuffer);
        if (n == NULL)
          return NULL;
        n->nextTab = NULL;
        n->currentBuffer = NULL;
        n->firstBuffer = NULL;
        return n;
      }

  inline int charColorToColor(int charColor) {
    switch(charColor) {
    case COL_FBLACK:
      return Color::black;
    case COL_FRED:
      return Color::red;
    case COL_FGREEN:
      return Color::green;
    case COL_FYELLOW:
      return Color::yellow;
    case COL_FBLUE:
      return Color::blue;
    case COL_FMAGENTA:
      return Color::magenta;
    case COL_FCYAN:
      return Color::cyan;
    case COL_FWHITE:
      return Color::white;
    default:
      MONA_TRACE("unknow color\n");
      return 0; // transparent
    }
  }


  void setAttribute(Graphics *g, l_prop attr)
    {
      if(attr & COL_BCOLOR) {
        setBackground(charColorToColor((attr&COL_BCOLOR) >> 8));
      } else {
        setBackground(_defaultBG);
      }
      if(attr & COL_FCOLOR) {
        setForeground(charColorToColor(attr&COL_FCOLOR));
      } else {
        setForeground(_defaultFG);
      }

      if(attr & S_BOLD) {
        g->setFontStyle(Font::FIXED & Font::BOLD);
      } else {
        g->setFontStyle(Font::FIXED);
      }

        _underLine = attr & S_UNDERLINE;
    }

  


  void initW3M() {
    int w = getWidth();
    int h = getHeight();

    int fw = colWidth();
    int fh = colHeight();

    COLS = (w-_xoffset*2)/fw;
    LINES = (h-_yoffset*2 - ((int)(fh/2.0)))/fh-1;

    init_rc();
    accept_cookie = true;
    use_cookie = true;

    wtf_init(DocumentCharset, DisplayCharset);

    setupscreen();
    sync_with_option();

    fmInit2();

    FirstTab = LastTab = CurrentTab = newTab();
  }
  virtual void processEvent(Event* event);

  int sameAttrLen(l_prop *pr, int len)
      {
        assert(len != 0);
        l_prop attr = pr[0] & ~C_WHICHCHAR; // donot care C_WCHAR* flag
        int i = 0; 
        for(i = 0;i < len && (pr[i] & ~C_WHICHCHAR  )== attr; i++);
        // for(i = 0;i < len && pr[i] == attr; i++);
        return i;
      }

  // return drawn width
  int drawStringWithAttr(Graphics *g, String s, l_prop attr, int x, int y)
    {
      int fh = colHeight();
      setAttribute(g, attr);
      int drawnWidth =  getFontMetrics()->getWidth(s);

        g->setColor(getBackground());
          
      g->fillRect(x, y, drawnWidth, fh);
      
      g->setColor(getForeground());
      g->drawString(s, x, y);
      if(_underLine) 
        g->drawLine(x, y+fh-2, x+drawnWidth, y+fh-2);
      return drawnWidth;
      
    }


  void drawString(Graphics *g, Screen* line, int x_org, int y) {
      if (!line->isdirty) {
          logprintf("omit drawing\n");
          return;
      } else {
          logprintf("dirty!!!\n");
      }
    char** pc = line->lineimage;
    l_prop *pr = line->lineprop;
    int same_attr_len = 0;
    int remain_len = COLS;
    int cur=0;
    int x = x_org;
    
    while(remain_len > 0) {
        l_prop attr = pr[cur];
        same_attr_len = sameAttrLen(&pr[cur], remain_len);
        Str utf8str = wtf_to_utf8(&pc[cur], &pr[cur], same_attr_len);
        String s(utf8str->ptr, utf8str->length);

        int drawnWidth = drawStringWithAttr(g, s, attr, x, y);
        x += drawnWidth;
        
        remain_len -= same_attr_len;
        cur += same_attr_len;

        while(CHMODE(pr[cur]) == C_WCHAR2) {
            cur++;
            remain_len--;
        }
    }
    line->isdirty = 0;
  }
    
  void clear(Graphics *g) {
    int w = getWidth();
    int h = getHeight();
    g->setColor(getBackground());
    g->fillRect(0, 0, w, h);
  }

  virtual void paint(Graphics* g) {
      uint64_t start = MonAPI::Date::nowInMsec();
    static bool first_time = true;
    if(first_time) {
      clear(g);
      first_time = false;
    }

    int fh = colHeight();


    g->setColor(getForeground());

    if(ScreenImage != NULL)
      {
        for(int line = 0; line < LINES; line++) {
          drawString(g, ScreenImage[line],  _xoffset, line*fh+_yoffset);
        }
      }
      drawCursor(g, Currentbuf->cursorX, Currentbuf->cursorY);
      uint64_t end = MonAPI::Date::nowInMsec();
      logprintf("end - start = %d\n", end -start);
  }
    void drawCursor(Graphics *g, int col, int row)
    {
        g->invartRect(_xoffset+col*colWidth(), _yoffset+colHeight()*row, colWidth(), colHeight());
    }
};


class W3MFrame: public Frame {
public:
  W3MFrame() : m_isAuto(false) {
    setBounds((800 - 640) / 2, (600 - 480) / 2, 640, 480);
    setTitle("w3m");
    m_pane = new W3MPane();
    m_pane->setBounds(0, 0, 640, 480);
    add(m_pane);
  }

  ~W3MFrame(){
      delete(m_pane);
  }

  void updatePane() {
    if(this->__g != NULL)
      m_pane->repaint(); // very slow.
  }

  void setScreenImage(Screen** si) {
     m_pane->ScreenImage = si;
  }

  // just copy pointer. be careful!
  void autoPilot(char *url) {
     m_isAuto = true;
     m_autoUrl = url;
  }


  void initW3M(char* url) {
    m_pane->initW3M();

    Buffer *newbuf = NULL;
    newbuf = loadGeneralFile(url, NULL, NO_REFERER, 0, NULL);
    Firstbuf = Currentbuf = newbuf;

    if(newbuf == NULL)
      MONA_TRACE("newbuf == null\n");

    displayBuffer(newbuf, B_FORCE_REDRAW);
    if(m_isAuto)
      setTimer(100);
  }

  virtual void processEvent(Event* event) ;


private: 
    W3MPane *m_pane;
    char *m_autoUrl;
    bool m_isAuto;
};



