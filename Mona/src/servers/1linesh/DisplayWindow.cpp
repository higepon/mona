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
  this->posY = 560;
  this->width = 800;
  this->height = 40;
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

int DisplayWindow::DrawCommandLine(const Charing c){

  int i, j;
  char *tmp;
  int pos;
  int x, y;
  Charing ss("1Lsh> ");

  this->ClearCommandLine();
  ss += c;
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

  commandWindow.fillRect16(this->posX, this->posY, this->width, 21, Color::rgb(0x00, 0x00, 0x00));
  return 0;
}

int DisplayWindow::DrawCommandWindow(){

  commandWindow.fillRect16(this->posX, this->posY, this->width, this->height, Color::rgb(0x00, 0x00, 0x00));
  this->DrawCommandLine("");
  this->DrawCursor(0);
  this->DrawMessageLine("");
  return 0;
}

int DisplayWindow::ClearCommandWindow(){

  commandWindow.fillRect16(this->posX, this->posY, this->width, this->height, Color::rgb(0x00, 0x00, 0x00));
  return 0;
}

int DisplayWindow::DrawCursor(int pos){

  commandWindow.fillRect16(this->posX+2+8*(pos+7), this->posY+18, 8, 2, Color::rgb(0xFF, 0xFF, 0xFF));
  return 0;
}

int DisplayWindow::DrawMessageLine(const Charing c){

  int i, j;
  char *tmp;
  int pos;
  int x, y;
  Charing ss(" Msg: ");

  this->ClearMessageLine();
  ss += c;
  pos = this->posX + 2;
  tmp = (char *)ss;
  while(*tmp != NULL){
    pos += 8;
    y = this->posY+22;
    for(i = 0; i < 16; i++){
      y = i + this->posY + 22;
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

int DisplayWindow::ClearMessageLine(){

  commandWindow.fillRect16(this->posX, this->posY+22, this->width, 18, Color::rgb(0x00, 0x00, 0x00));
  return 0;
}
