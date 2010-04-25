#pragma once

#include <baygui.h>

extern "C" {

typedef unsigned short l_prop;
typedef struct scline {
    char *lineimage;
    l_prop *lineprop;
    short isdirty;
    short eol;
} Screen;

}

class W3MPane: public Component {
public:
  Screen **ScreenImage;
  static const int _xoffset = 10;
  static const int _yoffset = 10;

  W3MPane() {
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


  void initW3M() {
    int w = getWidth();
    int h = getHeight();

    int fw = colWidth();
    int fh = colHeight();

    COLS = (w-_xoffset*2)/fw;
    LINES = (h-_yoffset*2)/fh;

    setupscreen();
    // sync_with_option();

    fmInit();

    FirstTab = LastTab = CurrentTab = newTab();
  }
  virtual void processEvent(Event* event);

  virtual void paint(Graphics* g) {

    int w = getWidth();
    int h = getHeight();

    g->setColor(getBackground());
    g->fillRect(0, 0, w, h);

    int fh = colHeight();
    g->setColor(getForeground());

    if(ScreenImage != NULL)
      {
        MONA_TRACE("repaint2\n");
        for(int line = 0; line < LINES; line++) {
          char* pc = ScreenImage[line]->lineimage;

          String s(&pc[0], COLS);
          g->drawString(s, _xoffset, line*fh+_yoffset);
        }
      }
  }
};


class W3MFrame: public Frame {
public:
  W3MFrame() {
     setBounds((800 - 212) / 2, (600 - 50) / 2, 640, 480);
     setTitle("w3m");
    m_pane = new W3MPane();
    m_pane->setBounds(0, 0, 640, 480);
    m_pane->setForeground(Color::navy);
    m_pane->setBackground(Color::white);
    add(m_pane);
/*
     m_label = new Label(m_time, Label::CENTER);
     m_label->setBounds(0, 4, 600, 400);
     add(m_label);
*/
  }

  ~W3MFrame(){
      delete(m_pane);
  }

  void updatePane() {
    if(this->__g != NULL)
      m_pane->update();
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

    MONA_TRACE("initW3M:begin display buffer\n");
    displayBuffer(newbuf, B_FORCE_REDRAW);
    MONA_TRACE("initW3M:end display buffer\n");
  }


private: 
    W3MPane *m_pane;
};



