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
     setBounds((800 - 212) / 2, (600 - 50) / 2, 640, 480);
     setTitle("w3m");
     initW3M();
     m_label = new Label(m_time, Label::CENTER);
     m_label->setBounds(0, 4, 200, 16);
     add(m_label);
  }

  ~W3MFrame(){
      delete(m_label);
  }

  virtual void repaint() {
    if (this->__g == NULL) return;

    int w = getWidth();
    int h = getHeight();

    int fw = getFontMetrics()->getWidth("A")*2; // tekito-

    for(int line = 0; line <= LINES; line++) {
      char* pc = ScreenImage[line]->lineimage;
      for(int col = 0; col < COLS; col++) {
        String s(&pc[col], 1);
        __g->drawString(s, col*fw+5, line*fw+5);
      }
    }
    Container::repaint();
  }

  void initW3M() {
      COLS = 50;
      LINES = 50;
      Buffer *newbuf = NULL;
      newbuf = loadGeneralFile("http://www.google.com", NULL, NO_REFERER, 0, NULL);
      Currentbuf = newbuf;
      // displayBuffer(Currentbuf, B_FORCE_REDRAW);
  }

  Screen **ScreenImage;

private: 
    char m_time[128];
    Label *m_label;
};



