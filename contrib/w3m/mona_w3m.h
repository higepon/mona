#pragma once

#include <baygui.h>

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

  void initW3M() {
      COLS = 50;
      LINES = 50;
      Buffer *newbuf = NULL;
      newbuf = loadGeneralFile("http://www.google.com", NULL, NO_REFERER, 0, NULL);
      Currentbuf = newbuf;
      displayBuffer(Currentbuf, B_FORCE_REDRAW);
  }

private: 
    char m_time[128];
    Label *m_label;
};




