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
#include <monalibc.h>

int uitos(char* s, unsigned int n, int real_width, int base, char flag);


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

  if(s == NULL) return result;
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

int itos(char *s, int n, int width, int base, char flag){
  int num;
  int real_width;
  int i;
  int j = 0;
  char charP = '+';
  
  if(s == NULL){
    return 0;
  }

  if(!(flag & P_FORMAT_UNSIGNED) && (n < 0)){/* negative number */
    flag |= P_FORMAT_PLUS;
    charP = '-';
    n = n * -1;
  }
  num = n;

  if((flag & P_FORMAT_SPACE) && !(flag & P_FORMAT_PLUS)){
    flag |= P_FORMAT_PLUS;
    charP = ' ';
  }

  for(real_width = 0; num != 0; real_width++){
    num /= base;
  }
  
  if((flag & P_FORMAT_ZERO) && (real_width >= width)){
    flag &= ~P_FORMAT_ZERO;
  }

  if(flag & P_FORMAT_PLUS){
    if(flag & P_FORMAT_MINUS){
      s[j] = charP;
      j++;
      j += uitos(&s[j], n, real_width, base, flag);
      for(i = 0; j < width; i++){
        s[j] = ' ';
         j++;
      }
    } else if(flag & P_FORMAT_ZERO){
      s[j] = charP;
      j++;
      for(i = 0; j < width - real_width; i++){
        s[j] = '0';
         j++;
      }
      j += uitos(&s[j], n, real_width, base, flag);
    } else {
      for(i = 0; j < width - real_width - 1; i++){
        s[j] = ' ';
         j++;
      }
      s[j] = charP;
      j++;
      j += uitos(&s[j], n, real_width, base, flag);
    }
  } else {
    if(flag & P_FORMAT_MINUS){
      j += uitos(&s[j], n, real_width, base, flag);
      for(i = 0; j < width; i++){
        s[j] = ' ';
         j++;
      }
    } else if(flag & P_FORMAT_ZERO){
      for(i = 0; j < width - real_width; i++){
        s[j] = '0';
         j++;
      }
      j += uitos(&s[j], n, real_width, base, flag);
    } else {
      for(i = 0; j < width - real_width; i++){
        s[j] = ' ';
         j++;
      }
      j += uitos(&s[j], n, real_width, base, flag);
    }
  }
  if(flag & P_FORMAT_TERMINATE){
    s[j] = 0;
    j++;
  }
  return j;
}

int uitos(char* s, unsigned int n, int real_width, int base, char flag){
  int j = 0;
  int i;
  size_t ch;
  char basechar = 'a';

  if(s == NULL) return 0;

  if(flag & P_FORMAT_CAPITAL){
    basechar = 'A';
  }

  for(i = real_width -1; i >= 0; i--){
    ch = n / __power(base, i);
    n %= __power(base, i);

    if(i == 0 && n > 9){
      s[j] = (basechar + n -10);
      j++;
    } else if (i == 0){
      s[j] = ('0' + n);
      j++;
    } else if (ch > 9){
      s[j] = (basechar + ch -10);
      j++;
    } else {
      s[j] = ('0' + ch);
      j++;
    }
  }

  return j;
}

int ftos(char *s, double n, int width, int precision, char flag){
  int num, fraction;
  int j = 0;
  char tmpflag = 0;

  if(s == NULL){
    return 0;
  }

  num = (int)n;
  fraction = (int)((n - (double)num)*__power(10, precision));
  if(precision > 0){
    if(flag & P_FORMAT_TERMINATE){
      flag &= ~P_FORMAT_TERMINATE;
      tmpflag = P_FORMAT_TERMINATE;
    }
    if(num != 0){
     width -= (precision + 1);
     j += itos(s, num, width, 10, flag);
     s[j] = '.';
     j++;
     if(fraction < 0) fraction *= -1;
     j += itos(&s[j], fraction, precision, 10, tmpflag);
    } else { /* num == 0 */
      if(fraction >= 0){
       s[j++] = '0';
       s[j++] = '.';
       j += itos(&s[j], fraction, precision, 10, tmpflag);
      } else { /* num == 0 && fraction < 0 */
       s[j++] = '-';
       s[j++] = '0';
       s[j++] = '.';
       fraction *= -1;
       j += itos(&s[j], fraction, precision, 10, tmpflag);
      }
    }
  } else { /* precision <= 0 */
    j += itos(s, num, width, 10, flag);
  }

  return j;
}

