/*!
  \file   DisplayWindow.cpp
  \brief  one line shell command

  Copyright (c) 2002-2004 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/04/04
  \author  shadow

  $Revision$
  $Date$
*/
#include <monapi.h>
#include "DisplayWindow.h"
#include "Charing.h"
#include "mona8x16.fnt"

using namespace MonAPI;

DisplayWindow::DisplayWindow(){

  this->posX = 0;
  this->posY = 550;
  this->width = 800;
  this->height = 50;
  this->mode =0;
  
  return;
}

DisplayWindow::~DisplayWindow(){

  return;
}

int DisplayWindow::SetWindowPos(int x, int y, int w, int h){

  this->posX = x;
  this->posY = y;
  this->width = w;
  this->height = h;
  return 0;
}

int DisplayWindow::DrawCommandLine(const Charing s){

  int i, j;
  char *tmp;
  int pos;
  int x, y;
  Charing ss("1Lsh> ");

  this->ClearCommandLine();
  ss += s;
  pos = this->posX + 2;
  tmp = (char *)ss;
  while(*tmp != NULL){
    pos += 8;
    y = this->posY+2;
    for(i = 0; i < 16; i++){
      y = i + this->posY + 2;
      for(j = 7; j >= 0; j--){
        if(((asciiMap[(int)*tmp][i] >> j) & 0x1) == 1){
          x = pos + 7 - j;
          commandWindow.putPixel16(x, y, Color::rgb(0xFF, 0xFF, 0xFF));
        }
      }
    }
    tmp++;
  }
  return 0;
}

int DisplayWindow::ClearCommandLine(){

  commandWindow.fillRect16(this->posX, this->posY, this->width, this->height, Color::rgb(0x00, 0x00, 0x00));
  return 0;
}

int DisplayWindow::DrawCommandWindow(){

  commandWindow.fillRect16(this->posX, this->posY, this->width, this->height, Color::rgb(0x00, 0x00, 0x00));
  this->DrawCommandLine("");
  return 0;
}

int DisplayWindow::ClearCommandWindow(){

  commandWindow.fillRect16(this->posX, this->posY, this->width, this->height, Color::rgb(0x00, 0x00, 0x00));
  return 0;
}

