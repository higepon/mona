/*!
  \file   MONAmona.cpp
  \brief  MONAmona based brainfuck

  Copyright (c) 2002-2004 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/06/08
  \author  shadow

  $Revision$
  $Date$
*/
#include <monapi.h>
#include "BfMain.h"
//#include <stdio.h>

BfMain::BfMain(){

  memset(mainbuf, 0, sizeof(mainbuf));
  mainptr = mainbuf;

  return;
}

BfMain::~BfMain(){

  return;
}

void BfMain::Init(void){

  memset(mainbuf, 0, sizeof(mainbuf));
  mainptr = mainbuf;

  return;
}

int BfMain::Parse(char *str){
  
  strptr = str;
  while(*strptr){
    switch(*strptr){
      case BF_GREATER: BfGreater(); break;
      case BF_LESS: BfLess(); break;
      case BF_PLUS: BfPlus(); break;
      case BF_MINUS: BfMinus(); break;
      case BF_COMMA: BfComma(); break;
      case BF_PERIOD: BfPeriod(); break; 
      case BF_LEFT: BfLeft(); break;
      case BF_RIGHT: BfRight(); break;
    }
    strptr++;
  }
  return 0; /* NEDD modify errors */
}

int BfMain::BfGreater(void){

  mainptr++;
  
  return 0; /* NEDD modify errors */
}

int BfMain::BfLess(void){

  mainptr--;

  return 0; /* NEDD modify errors */
}

int BfMain::BfPlus(void){

  (*mainptr)++;
  
  return 0; /* NEDD modify errors */
}

int BfMain::BfMinus(void){

  (*mainptr)--;
  
  return 0; /* NEDD modify errors */
}

int BfMain::BfComma(void){
  
  return 0; /* NEDD modify errors */
}

int BfMain::BfPeriod(void){

  printf("%c", *mainptr);
  
  return 0; /* NEDD modify errors */
}

int BfMain::BfLeft(void){

  if(*mainptr == 0){
    while (*(strptr++) != BF_RIGHT);
  }

  return 0; /* NEDD modify errors */
}

int BfMain::BfRight(void){

  if(*mainptr != 0) while (*(strptr--) != BF_LEFT);
  
  return 0; /* NEDD modify errors */
}

