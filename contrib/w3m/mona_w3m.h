#pragma once

#include <baygui.h>

extern "C" {
#include <assert.h>

typedef unsigned short l_prop;
typedef struct scline {
    char *lineimage;
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

    setupscreen();
    // sync_with_option();

    fmInit();

    FirstTab = LastTab = CurrentTab = newTab();
  }
  virtual void processEvent(Event* event);

  int sameAttrLen(l_prop *pr, int len)
      {
        assert(len != 0);
        l_prop attr = pr[0];
        int i = 0; 
        for(i = 0;i < len && pr[i] == attr; i++);
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
    if(!(line->isdirty & L_DIRTY))
      return;
    line->isdirty &= ~L_DIRTY;

    char* pc = line->lineimage;
    l_prop *pr = line->lineprop;
    int same_attr_len = 0;
    int remain_len = COLS;
    int cur=0;
    int x = x_org;
    
    while(remain_len > 0) {
      l_prop attr = pr[cur];
      same_attr_len = sameAttrLen(&pr[cur], remain_len);
      String s(&pc[cur], same_attr_len);

      int drawnWidth = drawStringWithAttr(g, s, attr, x, y);
      x += drawnWidth;
      
      remain_len -= same_attr_len;
      cur += same_attr_len;
    }

    MONA_TRACE_FMT((stderr, "update, x=%d, y=%d, w=%d, h=%d\n", x_org, y, x-x_org, colHeight()));
    update(x_org, y, x-x_org, colHeight());
  }

  void clear(Graphics *g) {
    int w = getWidth();
    int h = getHeight();
    g->setColor(getBackground());
    g->fillRect(0, 0, w, h);
  }

  void updatePane() {
    paint(getGraphics());
  }

  virtual void update(int x_org, int y_org, int w, int h)
    {
      Frame* c = (Frame *)getMainWindow();
      Graphics *g = c->getGraphics();
      int paneLeft = getX();
      int paneTop = getY();
      Image *buffer = getBuffer();

      int subOrgX = paneLeft+x_org;
      int subOrgY = paneTop + y_org;

      for(int j = 0; j < h; j++)
        {
          for(int i =0; i < w; i++)
            {
              g->drawPixel(subOrgX+i, subOrgY+j, buffer->getPixel(x_org+i, y_org+j));
            }
        }
      c->update(c->getX() + c->getInsets()->left + x_org, c->getY() + c->getInsets()->top + y_org, w, h);
    }

  virtual void paint(Graphics* g) {
    static bool first_time = true;
    if(first_time) {
      clear(g);
      first_time = false;
    }

    int fh = colHeight();


    g->setColor(getForeground());

    if(ScreenImage != NULL)
      {
        MONA_TRACE("repaint2\n");
        for(int line = 0; line < LINES; line++) {
          drawString(g, ScreenImage[line],  _xoffset, line*fh+_yoffset);
        }
      }
  }
};


class W3MFrame: public Frame {
public:
  W3MFrame() {
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
      m_pane->updatePane(); // TODO: should track inval rect, now call update directry inside paint.
  }

  void setScreenImage(Screen** si) {
     m_pane->ScreenImage = si;
  }


  void initW3M(char* url) {
    m_pane->initW3M();

    Buffer *newbuf = NULL;
    newbuf = loadGeneralFile(url, NULL, NO_REFERER, 0, NULL);
    Firstbuf = Currentbuf = newbuf;

    if(newbuf == NULL)
      MONA_TRACE("newbuf == null\n");

    displayBuffer(newbuf, B_FORCE_REDRAW);
  }


private: 
    W3MPane *m_pane;
};



