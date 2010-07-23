#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include <fenv.h>
#include <errno.h>
#include <inttypes.h>

int __hexchar2int(char c)
{
  if( isdigit(c) ) return (int)(c - '0');
  if( isupper(c) ) return (int)(c - 'A')+10;
  return (int)(c - 'a')+10;
}

int ___strtod_hex_a(char *p, char **ep)
{
  char c;
  int t = 0;
  int sign = 1;
  if( *p == '-' ){ sign = -1; p++; }
  do
  {
    c = *p;
    if( !isxdigit(c) ) break;
    t = 16*t+__hexchar2int(c);
  } while ( *p++ != NULL );
  *ep = p;
  return t*sign;
}
double ___strtod_hex_b(char *p, char **ep)
{
  char c;
  double factor = 0.0625;
  double t = 0.0;
  do
  {
    c = *p;
    if( !isxdigit(c) ) break;
    t = t+factor*(double)__hexchar2int(c);
    factor *= 0.0625;
  } while ( *p++ != NULL );
  *ep = p;
  return t;
}

int ___strtod_dec_a(char *p, char **ep);
double ___strtod_hex(char *p, char **endptr, const char *nptr)
{
  int rad_int = 0;
  double rad_r = 0.0;
  double r = 0.0;
  int e = 0;
  char c;
  char *ep;
  int sign = 1;
  if( *p == '-' ){ sign = -1; p++; }
  p += 2;

  rad_int = ___strtod_hex_a(p, &ep);
  if( *ep == '.' )
  {
    rad_r = ___strtod_hex_b(ep+1, &ep);
  }
  if( *ep == 'P' || *ep == 'p' )
  {
    e = ___strtod_dec_a(ep+1, &ep);
  }
  if( endptr ) *endptr = ep;
  r = ((double)rad_int+rad_r)*(double)sign*pow(2, e);
  if( isinf(r) )
  {
    errno = ERANGE;
    if( endptr ) *endptr = (char*)nptr;
    return (double)sign*HUGE_VAL;
  }
  return r;
}

int ___strtod_dec_a(char *p, char **ep)
{
  char c;
  int t = 0;
  int sign = 1;
  if( *p == '-' ){ sign = -1; p++; }
  else if( *p == '+' ){ sign = 1; p++; }
  do
  {
    c = *p;
    if( !isdigit(c) ) break;
    t = 10*t+(int)(c-'0');
  } while ( *p++ != NULL );
  *ep = p;
  return t*sign;
}
double ___strtod_dec_b(char *p, char **ep)
{
  char c;
  uint64_t factor = 1;
  double div = 0.0;
  uint64_t t = 0;
  int digits = 0;
  do
  {
    c = *p;
    if( !isdigit(c) ) break;
    t = t*10+(unsigned int)(c-'0');
    factor = factor*10;
    digits++;
    if( digits == DECIMAL_DIG )
    {
      while( isdigit(*++p) ); // skip to the end of the digits
      break;
    }
  } while ( *p++ != NULL );
  *ep = p;
  return (double)t/(double)factor;
}

double ___strtod_dec(char *p, char **endptr, const char *nptr)
{
  int rad_int = 0;
  double rad_r = 0.0;
  double r = 0.0;
  int e = 0;
  char c;
  char *ep;
  int sign = 1;
  if( *p == '-' ){ sign = -1; p++; }
  else if( *p == '+' ){ sign = 1; p++; }

  rad_int = ___strtod_dec_a(p, &ep);
  if( *ep == '.' )
  {
    rad_r = ___strtod_dec_b(ep+1, &ep);
  }
  if( *ep == 'E' || *ep == 'e' )
  {
    e = ___strtod_dec_a(ep+1, &ep);
  }
  if( endptr ) *endptr = ep;
  r = ((double)rad_int+rad_r)*(double)sign*pow(10, e);
  if( isinf(r) )
  {
    errno = ERANGE;
    if( endptr ) *endptr = (char*)nptr;
    return (double)sign*HUGE_VAL;
  }
  return r;
}
 
double strtod(const char *nptr, char **endptr)
{
  char* p = (char*)nptr;
  if( p == NULL ) return 0.0;

  while( isspace(*p) ) ++p; // skip white spaces

  if( strncasecmp(p, "INFINITY", 8) == 0 )
  {
    if( endptr != NULL ) *endptr = (char*)p+8;
    return +INFINITY;
  }
  if( strncasecmp(p, "+INFINITY", 9) == 0 )
  {
    if( endptr != NULL ) *endptr = (char*)p+9;
    return +INFINITY;
  }
  if( strncasecmp(p, "INF", 3) == 0 )
  {
    if( endptr != NULL ) *endptr = (char*)p+3;
    return +INFINITY;
  }
  if( strncasecmp(p, "+INF", 4) == 0 )
  {
    if( endptr != NULL ) *endptr = (char*)p+4;
    return +INFINITY;
  }
  if( strncasecmp(p, "-INFINITY", 9) == 0 )
  {
    if( endptr != NULL ) *endptr = (char*)p+9;
    return -INFINITY;
  }
  if( strncasecmp(p, "-INF", 4) == 0 )
  {
    if( endptr != NULL ) *endptr = (char*)p+4;
    return -INFINITY;
  }
  if( strncasecmp(p, "NAN", 3) == 0 )
  {
    if( endptr != NULL ) *endptr = (char*)p+3;
    return NAN;
  }
  if( strncasecmp(p, "0X", 2) == 0 )
  {
    return ___strtod_hex(p, endptr, nptr);
  }
  if( isdigit(*p) || *p == '+' || *p == '-' )
  {
    return ___strtod_dec(p, endptr, nptr);
  }
  if( endptr != NULL ) *endptr = p;
  return 0.0;
}

