/*!
  \file   vsprintf.cpp
  \brief  vsprintf

  Copyright (c) 2002-2004 shadow, Higepon
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/03/13
  \author  shadow

  $Revision: $
  $Date: 2007/12/07$ modified for kernel by Higepon.
*/

#include "kernel.h"
#include "stdarg.h"

#define P_FLAG_WIDTH 0x01
#define P_FLAG_PRECISION 0x02

#define P_FORMAT_ZERO 0x01
#define P_FORMAT_MINUS 0x02
#define P_FORMAT_PLUS 0x04
#define P_FORMAT_SPACE 0x08
#define P_FORMAT_SHARP 0x10
#define P_FORMAT_UNSIGNED 0x20
#define P_FORMAT_CAPITAL 0x40
#define P_FORMAT_TERMINATE 0x80

#define FLT_ROUNDS    1
#define FLT_RADIX     2

/* FLT_RADIX DIGit in MANTissa */
#define FLT_MANT_DIG  24
#define DBL_MANT_DIG  53
#define LDBL_MANT_DIG 53

/* decimal DIGit precision */
#define FLT_DIG   6
#define DBL_DIG  15
#define LDBL_DIG 15

/* distinction limit: min e : 1+e != 1 */
#define FLT_EPSILON  1.192092896e-07F
#define DBL_EPSILON  2.2204460492503131e-016
#define LDBL_EPSILON 2.2204460492503131e-016

/* min n : FLT_RADIX^n is expressible as float */
#define FLT_MIN_EXP   -125
#define DBL_MIN_EXP  -1021
#define LDBL_MIN_EXP -1021

/* max n : FLT_RADIX^n is expressible as float */
#define FLT_MAX_EXP   128
#define DBL_MAX_EXP  1024
#define LDBL_MAX_EXP 1024

/* min n : 10^n is expressible as float */
#define FLT_MIN_10_EXP   -37
#define DBL_MIN_10_EXP  -307
#define LDBL_MIN_10_EXP -307

/* max n : 10^n is expressible as float */
#define FLT_MAX_10_EXP   38
#define DBL_MAX_10_EXP  308
#define LDBL_MAX_10_EXP 308

/* min of float */
#define FLT_MIN  1.175494351e-38F
#define DBL_MIN  2.2250738585072014e-308
#define LDBL_MIN 2.2250738585072014e-308

/* max of float */
#define FLT_MAX  3.402823466e+38F
#define DBL_MAX  1.7976931348623158e+308
#define LDBL_MAX 1.7976931348623158e+308

#define HUGE_VAL	1e500
#define HUGE_VALF	1e50f
#define HUGE_VALL	1e5000
#define INFINITY	HUGE_VALF
#define LN2		0.6931471805599453094172321
#define NAPIER		2.71828182845904523536

#define M_E		2.7182818284590452354	/* e */
#define M_LOG2E		1.4426950408889634074	/* log 2e */
#define M_LOG10E	0.43429448190325182765	/* log 10e */
#define M_LN2		0.69314718055994530942  /* log e2 */
#define M_LN10          2.30258509299404568402  /* log e10 */
#define M_PI		3.14159265358979323846  /* pi */
#define M_PI_2		1.57079632679489661923  /* pi/2 */
#define M_PI_4		0.78539816339744830962  /* pi/4 */
#define M_1_PI		0.31830988618379067154  /* 1/pi */
#define M_2_PI		0.63661977236758134308  /* 2/pi */
#define M_2_SQRTPI	1.12837916709551257390  /* 2/sqrt(pi) */
#define M_SQRT2		1.41421356237309504880  /* sqrt(2) */
#define M_SQRT1_2	0.70710678118654752440  /* 1/sqrt(2) */

int strcpy2(char *s1, const char *s2){
  char *tmp = s1;

  while((*tmp++ = *s2++));

  return (int)(tmp - s1);
}

int strncpy2(char *s1, const char *s2, int n){
  char *tmp = s1;
  int result;

  while((n-- > 0) && (*tmp++ = *s2++));
  result = (int)(tmp - s1);
  while(n-- > 0) *tmp++ = '\0';

  return result;
}

double fabs(double x);
double pow(double x, double y);
int itosn(char *s, int max_width, int n, int width, unsigned int base, char flag);
int itos(char *s, int n, int width, unsigned int base, char flag);
/**
  \brief double to string

  \param s          string buffer printed characters
  \param n          number converted
  \param real_width width of number to be converted before decimal-point
  \param precision  width of number to be converted after decimal-point
  \param flag       conversion using flag
  \return size of characters printed to buffer
 */
int ftos(char *s, double n, int width, int precision, char flag) {
    int num, fraction;
    int j = 0;
    char tmpflag = 0;

    if(s == NULL) {
        return 0;
    }

    num = (int)n;
    fraction = (int)((n - (double)num)*pow(10, precision));
    if(precision > 0) {
        if(flag & P_FORMAT_TERMINATE) {
            flag &= ~P_FORMAT_TERMINATE;
            tmpflag = P_FORMAT_TERMINATE;
        }
        if(num != 0) {
            width -= (precision + 1);
            j += itos(s, num, width, 10, flag);
            s[j] = '.';
            j++;
            if(fraction < 0) fraction *= -1;
            j += itos(&s[j], fraction, precision, 10, tmpflag);
        } else { /* num == 0 */
            if(fraction >= 0) {
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



double modf(double x, double *buf)
{
    double a = fabs(x);
    int i = (int)x;

    double f = a - (double)i;

    *buf = x < 0 ? -i : i;

    return x < 0 ? -f : f;
}

double floor(double x)
{
    if (x < 0)
    {
        if (modf(-x, &x) != 0) x++;
        return -x;
    }
    modf(x, &x);
    return x;
}

double fabs(double x)
{
    return x < 0.0 ? -x : x;
}

double exp(double x){

  int ix, n = 1;
  double ax = fabs(x), a = 1.0;
  double tmp1;
  double result = 1.0;

  /* exp(2.3) = exp(2)*exp(0.3) */
  /* exp(x) = 1 + x^1/1! + x^2/2! + x^3/3! ... + x^k/k! */
  if(ax < DBL_EPSILON) return 1;
  ix = (int)floor(ax); /* integer part */
  ax -= ix;       /* fraction part */

  do{
    tmp1 = result;
    a *= ax/n;
    result += a;
    n++;
  } while(result != tmp1);

  a = NAPIER;
  while(ix > 0){
    if(ix&1){ /* ix is odd */
      if(x >= 0 && a > DBL_MAX/result) return HUGE_VAL;
      if(x < 0 && a > DBL_MAX/result) return 0;
      result *= a;
    }
    ix /= 2;
    a = a*a;
  }

  if(x >= 0) return result;
  else return 1.0/result;
}

double log(double x){

  int n = 1;
  double tmp1, tmp2, result = 0;

  if(x <= 0) return 0;
  while(x >= 2){ /* to x<2 */ /* ln(a)=ln(a/2)+ln(2) */
    x /= 2;
    result += LN2;
  }
  while(x < 1){  /* to x>=1 */ /* ln(a)= ln(2a)-ln(2) */
    x *= 2;
    result -= LN2;
  }
  /* 1<=x<2(?): ln(x) = sigma (2/(2k+1))*(((x-1)/(x+1))^(2k+1)) */
  x = (x-1)/(x+1);
  tmp1 = 2*x;
  x = x*x;
  do{
    tmp2 = result;
    result += tmp1/n;
    tmp1 *= x;
    n += 2;
  } while(result != tmp2);

  return result;
}

double pow(double x, double y){

    long int n = (long int)y;
    double result = 1;

  if(x == 0){
    if(y <= 0) return 1; /* 0^y = 1 (y <= 0) */
    return 0;            /* 0^y = 0 (y > 0)  */
  }

  if(y == n){ /* y is integer */
    char sign = 0;
    if(n < 0){
      sign = 1;
      n = -n;
      x = 1/x;
    }
    while(1){
      if(n&1) result *= x; /* n is odd */
      n /= 2;
      if(n == 0) break;
      x *= x;
    }
    return result;
  }

  if(x < 0) return 0; /* error: result is imaginary */

  return exp(y*log(x));
}

/*!
 * \brief unsigned int to string with max size n
 *
 * \param s          string buffer printed characters
 * \param max_width  maximum field width of number to be converted
 *                   (nearly equal string buffer size), 0< : infinity
 * \param n          number converted
 * \param real_width width of number to be converted
 * \param base       radix of number to be converted
 * \param flag       conversion using flag
 * \return size of characters printed to buffer
 */
int uitosn(char* s, int max_width, unsigned int n, int real_width, unsigned int base, char flag) {
    int j = 0;
    int i;
    size_t ch;
    char basechar = 'a';

    if((s == NULL) || (max_width == 0)) return 0;

    if(flag & P_FORMAT_CAPITAL) {
        basechar = 'A';
    }

    for(i = real_width -1; i >= 0; i--) {
        ch = (size_t)(n / pow(base, i));
        n %= (int)pow(base, i);

        if (ch > 9) {
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
 * \brief unsigned int to string
 *
 * \param s         string buffer printed characters
 * \param max_width maximum field width of number to be converted
 *                  (nearly equal string buffer size), 0< : infinity
 * \param n         number converted
 * \param width     minimum field width of number to be converted
 * \param base      radix of number to be converted
 * \param flag      conversion using flag
 * \return size of characters printed to buffer
 *
 */
int itosn(char *s, int max_width, int n, int width, unsigned int base, char flag) {
    int num;
    int real_width;
    int i;
    int j = 0;
    char charP = '+';

    if((s == NULL) || (max_width == 0) || (base == 0) || (base > 36)) {
        return 0;
    }

    if((max_width > 0) && (max_width < width)) width = max_width;

    if(!(flag & P_FORMAT_UNSIGNED) && (n < 0)) {/* negative number */
        flag |= P_FORMAT_PLUS;
        charP = '-';
        n = n * -1;
    }
    num = n;

    if((flag & P_FORMAT_SPACE) && !(flag & P_FORMAT_PLUS)) {
        flag |= P_FORMAT_PLUS;
        charP = ' ';
    }

    if(num == 0) {
        real_width = 1;
    } else {
        for(real_width = 0; num != 0; real_width++) {
            num /= base;
        }
    }

    if((flag & P_FORMAT_ZERO) && (real_width >= width)) {
        flag &= ~P_FORMAT_ZERO;
    }

    if(flag & P_FORMAT_PLUS) {
        if(flag & P_FORMAT_MINUS) {
            s[j] = charP;
            j++;
            j += uitosn(&s[j], max_width - j, n, real_width, base, flag);
            for(i = 0; j < width; i++) {
                s[j] = ' ';
                j++;
            }
        } else if(flag & P_FORMAT_ZERO) {
            s[j] = charP;
            j++;
            for(i = 0; j < width - real_width; i++) {
                s[j] = '0';
                j++;
            }
            j += uitosn(&s[j], max_width - j, n, real_width, base, flag);
        } else {
            for(i = 0; j < width - real_width - 1; i++) {
                s[j] = ' ';
                j++;
            }
            s[j] = charP;
            j++;
            j += uitosn(&s[j], max_width - j, n, real_width, base, flag);
        }
    } else {
        if(flag & P_FORMAT_MINUS) {
            j += uitosn(&s[j], max_width - j, n, real_width, base, flag);
            for(i = 0; j < width; i++) {
                s[j] = ' ';
                j++;
            }
        } else if(flag & P_FORMAT_ZERO) {
            for(i = 0; j < width - real_width; i++) {
                s[j] = '0';
                j++;
            }
            j += uitosn(&s[j], max_width - j, n, real_width, base, flag);
        } else {
            for(i = 0; j < width - real_width; i++) {
                s[j] = ' ';
                j++;
            }
            j += uitosn(&s[j], max_width - j, n, real_width, base, flag);
        }
    }
    if(flag & P_FORMAT_TERMINATE) {
        s[j] = 0;
        j++;
    }
    return j;
}

/**
 * \brief int to string
 *
 * \param s     string buffer printed characters
 * \param n     number converted
 * \param width minimum field width of number to be converted
 * \param base  radix of number to be converted
 * \param flag  conversion using flag
 * \return size of characters printed to buffer
 *
 */
int itos(char *s, int n, int width, unsigned int base, char flag) {
    return itosn(s, -1, n, width, base, flag);
}

/*!
  \brief equivalent to the function sprintf

  \param s      buf printed characters
  \param format specifies how subsequent arguments
  \param arg    a variable number of arguments
  \return  the number of characters printed (including '\\0' end of string)
*/
int vsprintf(char *s, const char *format, va_list arg) {
  int result = 0;
  int loop;
  int i;
  int width;
  char flag;
  int precision;
  char p_flag;

  if(s == NULL) return 0;
  for(i = 0; format[i] != '\0'; i++){
    if(format[i] == '%'){
      loop = 1;
      flag = 0;
      p_flag = 0;
      width = 0;
      precision = 0;
      while(loop){
        i++;
        switch(format[i]){
          case 's':
            result += strcpy2(&s[result], va_arg(arg, char *));
            result--;
            loop = 0;
            break;
          case 'd':
          case 'i':
            result += itos(&s[result], va_arg(arg, int), width, 10, flag);
            loop = 0;
            break;
          case 'o':
            flag |= P_FORMAT_UNSIGNED;
            result += itos(&s[result], va_arg(arg, int), width, 8, flag);
            loop = 0;
            break;
          case 'u':
            flag |= P_FORMAT_UNSIGNED;
            result += itos(&s[result], va_arg(arg, int), width, 10, flag);
            loop = 0;
            break;
          case 'X':
            flag |= P_FORMAT_CAPITAL;
          case 'x':
            flag |= P_FORMAT_UNSIGNED;
            result += itos(&s[result], va_arg(arg, int), width, 16, flag);
            loop = 0;
            break;
          case 'f':
            result += ftos(&s[result], va_arg(arg, double), width, precision, flag);
            loop = 0;
            break;
          case 'c':
            s[result++] = va_arg(arg, char);
            loop = 0;
            break;
          case '%':
            s[result++] = '%';
            loop = 0;
            break;
          case '-':
            flag |= P_FORMAT_MINUS;
            break;
          case '+':
            flag |= P_FORMAT_PLUS;
            break;
          case ' ':
            flag |= P_FORMAT_SPACE;
            break;
          case '0':
            if(!(p_flag & P_FLAG_WIDTH) && !(p_flag & P_FLAG_PRECISION)){
              flag |= P_FORMAT_ZERO;
              break;
            }
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
            if(p_flag & P_FLAG_PRECISION){
              precision = precision*10 + (int)(format[i] - '0');
            } else {
              width = width*10 + (int)(format[i] - '0');
              p_flag |= P_FLAG_WIDTH;
            }
            break;
          case '.':
            p_flag |= P_FLAG_PRECISION;
            break;
          case '\0':
            i--;
            loop = 0;
            break;
        }
      }
    } else {
      s[result++] = format[i];
    }
  }
  s[result] = 0;
  return result;
}

