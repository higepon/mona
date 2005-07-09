/*!aa
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
#include <monalibc.h>
#include <monalibc/stdlib.h>
#include <monapi/string.h>
#include <monalibc/ctype.h>

int uitos(char* s, unsigned int n, int real_width, unsigned int base, char flag);
int uitosn(char* s, int max_width, unsigned int n, int real_width, unsigned int base, char flag);


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

  head = s;
  if(*head == '0'){ /* modify base using s */
    head++;
    if( ((*head == 'X') || (*head == 'x')) && ((base == 0) || (base == 16)) ){
      head++;
      if(isxdigit(*head) != 0){
        base = 16;
        s = head; /* skip 0X or 0x */
      }
    } else if(*head >= '1' && *head <= '7'){
      if(base == 0) base = 8;
    } else {
      if(base == 0) base = 10;
    }
  } else if(base == 0){
    base = 10;
  }
  head = s;

  if(width == 0) width = INT_MAX;
  width -= (int)(head - tmp);
  if(flag & S_FORMAT_UNSIGNED){
    mflag = 1;
    if(flag & S_FORMAT_LONG){
      max = ULONG_MAX;
    } else {
      max = UINT_MAX;
    }
  } else {
    if(flag & S_FORMAT_LONG){
      max = LONG_MAX;
    } else {
      max = INT_MAX;
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

/*!
  \brief int to string

  \param s     string buffer printed characters
  \param n     number converted
  \param width minimum field width of number to be converted
  \param base  radix of number to be converted
  \param flag  conversion using flag
  \return size of characters printed to buffer
*/
int itos(char *s, int n, int width, unsigned int base, char flag){
  return itosn(s, -1, n, width, base, flag);
}

/*!
  \brief unsigned int to string

  \param s         string buffer printed characters
  \param max_width maximum field width of number to be converted
                   (nearly equal string buffer size), 0< : infinity
  \param n         number converted
  \param width     minimum field width of number to be converted
  \param base      radix of number to be converted
  \param flag      conversion using flag
  \return size of characters printed to buffer
*/
int itosn(char *s, int max_width, int n, int width, unsigned int base, char flag){
  int num;
  int real_width;
  int i;
  int j = 0;
  char charP = '+';
  
  if((s == NULL) || (max_width == 0) || (base == 0) || (base > 36)){
    return 0;
  }

  if((max_width > 0) && (max_width < width)) width = max_width;

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

  if(num == 0){
    real_width = 1;
  } else {
    for(real_width = 0; num != 0; real_width++){
      num /= base;
    }
  }

  if((flag & P_FORMAT_ZERO) && (real_width >= width)){
    flag &= ~P_FORMAT_ZERO;
  }

  if(flag & P_FORMAT_PLUS){
    if(flag & P_FORMAT_MINUS){
      s[j] = charP;
      j++;
      j += uitosn(&s[j], max_width - j, n, real_width, base, flag);
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
      j += uitosn(&s[j], max_width - j, n, real_width, base, flag);
    } else {
      for(i = 0; j < width - real_width - 1; i++){
        s[j] = ' ';
         j++;
      }
      s[j] = charP;
      j++;
      j += uitosn(&s[j], max_width - j, n, real_width, base, flag);
    }
  } else {
    if(flag & P_FORMAT_MINUS){
      j += uitosn(&s[j], max_width - j, n, real_width, base, flag);
      for(i = 0; j < width; i++){
        s[j] = ' ';
         j++;
      }
    } else if(flag & P_FORMAT_ZERO){
      for(i = 0; j < width - real_width; i++){
        s[j] = '0';
         j++;
      }
      j += uitosn(&s[j], max_width - j, n, real_width, base, flag);
    } else {
      for(i = 0; j < width - real_width; i++){
        s[j] = ' ';
         j++;
      }
      j += uitosn(&s[j], max_width - j, n, real_width, base, flag);
    }
  }
  if(flag & P_FORMAT_TERMINATE){
    s[j] = 0;
    j++;
  }
  return j;
}

/*!
  \brief unsigned int to string

  \param s          string buffer printed characters
  \param n          number converted
  \param real_width width of number to be converted
  \param base       radix of number to be converted
  \param flag       conversion using flag
  \return size of characters printed to buffer
*/
int uitos(char* s, unsigned int n, int real_width, unsigned int base, char flag){
  return uitosn(s, -1, n, real_width, base, flag);
}

/*!
  \brief unsigned int to string with max size n

  \param s          string buffer printed characters
  \param max_width  maximum field width of number to be converted
                    (nearly equal string buffer size), 0< : infinity
  \param n          number converted
  \param real_width width of number to be converted
  \param base       radix of number to be converted
  \param flag       conversion using flag
  \return size of characters printed to buffer
*/
int uitosn(char* s, int max_width, unsigned int n, int real_width, unsigned int base, char flag){
  int j = 0;
  int i;
  size_t ch;
  char basechar = 'a';

  if((s == NULL) || (max_width == 0)) return 0;

  if(flag & P_FORMAT_CAPITAL){
    basechar = 'A';
  }

  for(i = real_width -1; i >= 0; i--){
    ch = n / __power(base, i);
    n %= __power(base, i);

    if (ch > 9){
      s[j] = (basechar + ch -10);
      j++;
    } else {
      s[j] = ('0' + ch);
      j++;
    }
    if((max_width - j) == 0) break;
  }

  return j;
}

/*!
  \brief double to string

  \param s          string buffer printed characters
  \param n          number converted
  \param real_width width of number to be converted before decimal-point
  \param precision  width of number to be converted after decimal-point
  \param flag       conversion using flag
  \return size of characters printed to buffer
*/
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

/*!
  \brief absolute

  \param i int

  \return absolute value of i
*/
int abs(int i){

  return i >= 0 ? i :-i;
}

/*!
  \brief long absolute

  \param i long int

  \return absolute value of i
*/
long int labs(long int i){

  return i >= 0 ? i :-i;
}

/*!
  \brief integer division

  \param numer dividend
  \param denom divisor

  \return quotient and remainder
*/
div_t div(long int numer, long int denom){

  div_t result;
  ldiv_t tmp = ldiv(numer, denom);

  result.quot = tmp.quot;
  result.rem  = tmp.rem;

  return result;
}

/*!
  \brief long integer division

  \param numer dividend
  \param denom divisor

  \return quotient and remainder
*/
ldiv_t ldiv(long int numer, long int denom){

  ldiv_t result;
  long int anumer = labs(numer);
  long int adenom = labs(denom);

  result.quot = anumer / adenom;
  result.rem  = anumer % adenom;

  if((numer < 0 && denom > 0) || (numer > 0 && denom < 0)) result.quot = -result.quot;
  if(numer < 0) result.rem = -result.rem;

  return result;
}

/*!
  \brief quick sort

  \param base pointer to start of array
  \param n    number of elements of array
  \param size size of element
  \param fnc  pointer to comparison function

*/
void qsort(void *base, size_t n, size_t size, int (*fnc)(const void*, const void*)){

  char *stdv = (char *)base; /* pointer to comparison standard value */
  char *tmp = stdv + size;
  char *right;
  size_t i, exchange = 0;

  if(n <= 1) return;

  if(n == 2){
    if(fnc(stdv, tmp) > 0) memswap(stdv, tmp, size);
    return;
  }

  right = stdv;
  for(i = 1; i < n; i++){
    if(fnc(tmp, stdv) > 0){
      exchange++;
      right += size;
      if(tmp != right) memswap(tmp, right, size);
    }
    tmp += size;
  }

  if(stdv != right) memswap(stdv, right, size);
  qsort(base, exchange, size, fnc);
  qsort(right + size, n - exchange - 1, size, fnc);

  return;
}

/*!
  \brief binary search

  \param key  pointer to search value
  \param base pointer to start of array
  \param n    number of elements of array
  \param size size of element
  \param fnc  pointer to comparison function

  \return pointer to found element, NULL means Not found
*/
void *bsearch(const void *key, const void *base, size_t n, size_t size, int (*fnc)(const void *, const void *)){

  char *result;
  int cmp;
  size_t lo = 0, mid, hi = n - 1;

  do{
    mid = (lo + hi)/2;
    result = (char *)base + size*mid;
    cmp = fnc(key, result);
    if(cmp == 0){       /* key == result */
      while((result > base) && !fnc(key, result - size)) result -= size; /* search lower result */
      return result;
    } else if(cmp < 0){ /* key < result */
      hi = mid - 1;
    } else {  /* cmp > 0 : key > result */
      lo = mid + 1;
    }
  } while(lo <= hi);
  
  result = NULL;
  return result;
}

/*!
  \brief string do double 

  \param s  pointer to string to be converted
  \param endptr pointer to the following character of converted

  \return the converted value, if any.
*//*
double strtod(const char *s, char **endptr){

  double result = 0.0;

  return result;
}*/

float strtof(const char *nptr, char **endptr){

  float result = 0.0;

  return result;
}

long double strtold(const char *nptr, char **endptr){

  long double result = 0.0;

  return result;
}


void abort()
{
	exit(1);
}

char* getenv(const char* NAME)
{
	return NULL;
}

int putenv(char *string)
{
	return 0;
}

double strtod(const char *STR, char **TAIL)
{
	return atof(STR);
}

// from hige3d/notmine.cpp
double atof(const char *s)
{
	double a = 0.0;
	int e = 0;
	int c;
	int minus;

	c = *s;
	if (c == '-')
	{
		s++;
		minus = -1;
	}
	else
	{
		minus = 1;
	}

	while ((c = *s++) != '\0' && isdigit(c)) {
		a = a*10.0 + (c - '0');
	}
	if (c == '.') {
		while ((c = *s++) != '\0' && isdigit(c)) {
			a = a*10.0 + (c - '0');
			e = e-1;
		}
	}
	if (c == 'e' || c == 'E') {
		int sign = 1;
		int i = 0;
		c = *s++;
		if (c == '+')
			c = *s++;
		else if (c == '-') {
			c = *s++;
			sign = -1;
		}
		while (isdigit(c)) {
			i = i*10 + (c - '0');
			c = *s++;
		}
		e += i*sign;
	}
	while (e > 0) {
		a *= 10.0;
		e--;
	}
	while (e < 0) {
		a *= 0.1;
		e++;
	}
	return a * minus;
}

static double s_seed = 0.314159265359;

int rand()
{
	s_seed *= 17.0;
	s_seed -= (double)(int)s_seed;
	return (int)(s_seed * RAND_MAX);
}

void srand(unsigned seed)
{
	s_seed = ((double)(seed % RAND_MAX)) / ((double)RAND_MAX);
}


/*
#undef exit
static void(*_p_exit)(int code) = NULL;

void _mona_set_exit(void(*_exit)(int code))
{
	_p_exit = _exit;
}

void _mona_exit(int code)
{
	if (_p_exit == NULL)
	{
		exit(code);
	}
	else
	{
		(*_p_exit)(code);
	}
}*/
