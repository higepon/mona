/*!
  \file   Charing.cpp
  \brief  charing is like string

  Copyright (c) 2002-2004 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/03/27
  \author  shadow

  $Revision$
  $Date$
*/

#include "Charing.h"
#include <monapi/string.h>
#include <monalibc.h>

Charing::Charing() : maxsize(CHARSIZE-1){

  memset(this->str, '\0', CHARSIZE);
  this->length = 0;
  return;
}

Charing::Charing(const char s) : maxsize(CHARSIZE-1){

  memset(this->str, '\0', CHARSIZE);
  this->str[0] = s;
  this->length = 1;
  return;
}

Charing::Charing(const char *s) : maxsize(CHARSIZE-1){

  memset(this->str, '\0', CHARSIZE);
  strncpy2(this->str, s, CHARSIZE-1);
  this->length = strlen(this->str);
  return;
}

Charing::~Charing(){

  return;
}

int Charing::GetLength(){

  return this->length;
}

int Charing::Set(const char *s){

  int result;
  result = strncpy2(this->str, s, maxsize);
  this->length = strlen(this->str);
  return result;
}

int Charing::Setf(const char *format, ...){

  int result;
  va_list args;

  va_start(args, format);
  result = vsprintf(this->str, format, args);
  va_end(args);
  this->length = result;
  return result;
}

void Charing::Reset(){

  memset(this->str, '\0', maxsize);
  this->length = 0;
  return;
}

int Charing::Add(const char *s){

  int result;

  result = strncpy2(&(this->str[this->length]), s, maxsize - this->length);
  this->length = strlen(this->str);
  return result;
}

int Charing::Insert(int pos, const Charing& s){

  char tmp[CHARSIZE];
  int tmpsize, tmppos;

  if((this->length + s.length) <= this->maxsize){
    tmpsize = strncpy2(tmp, &(this->str[pos]), this->maxsize);
    tmppos = strncpy2(&(this->str[pos]), s.str, s.length);
    strncpy2(&(this->str[pos+tmppos]), tmp, tmpsize);
    this->length += s.length;
  } else {
    tmppos = -1;
  }
  return tmppos;
}

int Charing::Remove(int pos, int width){

  char tmp[CHARSIZE];
  int result;

  if((pos + width) <= this->maxsize){
    strncpy(tmp, &(this->str[pos+width]), maxsize);
    strncpy(&(this->str[pos]), tmp, maxsize - pos);
    this->length -= width;
    result = width;
  } else {
    memset(&(this->str[pos]), '\0', maxsize - pos);
    this->length = pos;
    result = maxsize - pos;
  }
  return result;
}

Charing Charing::operator+(const Charing& s){

  this->Add(s.str);
  return *this;
}

Charing& Charing::operator=(const char s){

  this->Set(&s);
  return *this;
}

Charing& Charing::operator=(const char *s){

  this->Set(s);
  return *this;
}

Charing& Charing::operator=(const Charing& s){

  this->Set(s.str);
  return *this;
}

Charing& Charing::operator+=(const Charing& s){

  this->Add(s.str);
  return *this;
}

Charing::operator char *(){

  return this->str;
}
