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



class W3MFrame: public Frame {
public:
  W3MFrame() {
    ScreenImage = NULL;
     setBounds((800 - 212) / 2, (600 - 50) / 2, 640, 480);
     setTitle("w3m");
     m_label = new Label(m_time, Label::CENTER);
     m_label->setBounds(0, 4, 200, 16);
     add(m_label);
  }

  ~W3MFrame(){
      delete(m_label);
  }

  static const int _xoffset = 10;
  static const int _yoffset = 10;

  virtual void repaint() {
  MONA_TRACE("repaint\n");
    if (this->__g == NULL) return;
/*
    int w = getWidth();
    int h = getHeight();
*/
  MONA_TRACE("repaint1.5\n");

    int fw = getFontMetrics()->getWidth("A"); // tekito-

    if(ScreenImage != NULL)
    {
  MONA_TRACE("repaint2\n");
//      for(int line = 0; line <= LINES; line++) {
      for(int line = 0; line <= 20; line++) {
        char* pc = ScreenImage[line]->lineimage;
        for(int col = 0; col < COLS; col++) {
          String s(&pc[col], 1);
          __g->drawString(s, col*fw+_xoffset, line*fw+_yoffset);
        }
      }
    }
    Container::repaint();
  }

  void initW3M() {
       Buffer *newbuf = NULL;
       newbuf = loadGeneralFile("http://www.nattou.org", NULL, NO_REFERER, 0, NULL);
       // Currentbuf = newbuf;
  MONA_TRACE("initW3M:begin display buffer\n");
// OK
       displayBuffer(newbuf, B_FORCE_REDRAW);
  MONA_TRACE("initW3M:end display buffer\n");
  }

  Screen **ScreenImage;

private: 
    char m_time[128];
    Label *m_label;
};



