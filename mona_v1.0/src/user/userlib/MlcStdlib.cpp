/*!
  \file   MlcStdlib.cpp
  \brief  mona c standard library

  Copyright (c) 2002-2004 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/02/29
  \author  shadow

  $Revision$
  $Date$
*/
#include <MlcLimits.h>
#include <MlcCtype.h>
#include <MlcStdlib.h>

/*!
  \brief string to long int

  \param s      string to be converted
  \param endptr pointer after string converted
  \param base   radix of string to be converted
  \return  result of the conversion
*/
long int strtol(const char *s, char **endptr, int base){
  return strtoi(s, endptr, base, 0, S_FORMAT_LONG);
}

/*!
  \brief string to unsigned long int

  \param s      string to be converted
  \param endptr pointer after string converted
  \param base   radix of string to be converted
  \return  result of the conversion
*/
unsigned long int strtoul(const char *s, char **endptr, int base){
  return strtoi(s, endptr, base, 0, S_FORMAT_LONG | S_FORMAT_UNSIGNED);
}


/*!
  \brief string to int

  \param s      string to be converted
  \param endptr pointer after string converted
  \param base   radix of string to be converted
  \param width  size of string to be converted
  \param flag   conversion using flag
  \return  result of the conversion
*/
size_t strtoi(const char *s, char **endptr, int base, int width, char flag){
  const char *tmp = s;
  const char *head;
  unsigned long int result = 0;
  int mflag = 1;
  unsigned long int max;

  if(base > 36) base = 36; /* check base */
  if(base < 0) base = 0;
  while(isspace(*s)) s++; /* skip spaces */
  if(*s == '+'){ /* init Minus flag */
    s++;
  } else if(*s == '-'){
    mflag = -1;
    s++;
  }
  if(*s == '0'){ /* modify base using s */
    s++;
    if(*s == 'X' || *s == 'x'){
      if((base == 0) || (base == 16)){
        base = 16;
        s++;
      }
    } else if(*s >= '1' && *s <= '7'){
      if(base == 0) base = 8;
    } else {
      if(base == 0) base = 10;
    }
  } else if(base == 0){
    base = 10;
  }
  head = s;
  if(width == 0) width = M_INT_MAX;
  width -= (int)(head - tmp);
  if(flag & S_FORMAT_UNSIGNED){
    mflag = 1;
    if(flag & S_FORMAT_LONG){
      max = M_ULONG_MAX;
    } else {
      max = M_UINT_MAX;
    }
  } else {
    if(flag & S_FORMAT_LONG){
      max = M_LONG_MAX;
    } else {
      max = M_INT_MAX;
    }
  }

  while(width-- > 0){ /* conversion */
    long int value;

    if(!isalnum(*s)) break;
    if(isdigit(*s)){
      value = *s - '0';
    } else {
      value = toupper(*s) - ('A' - 10);
    }
    if(value >= base) break;
    if(result == max){
      s++;
      continue;
    } else if(result > (max - value)/base){
      result = max;
      s++;
      continue;
    }
    result = result*base + value;
    s++;
  }

  if(endptr != NULL){ /* set endptr */
    if(s == head){
      *endptr = (char *)tmp;
    } else {
      *endptr = (char *)s;
    }
  }

  return result * (long)mflag;
}


/*!
  \brief ascii of decimal to int

  \param s string to be converted
  \return  result of the conversion
*/
int atoi(const char *s){
  int result;
  int mflag;

  if(s == NULL) return 0;
  mflag = 1;
  result = 0;

  while(isspace(*s)) s++;

  if(*s == '+'){
    mflag = 1;
    s++;
  } else if(*s == '-'){
    mflag = -1;
    s++;
  }
  
  while((*s >= '0') && (*s <= '9')){
    result = result*10 + (int)(*s - '0');
    s++;
  }

  return result*mflag;
}
