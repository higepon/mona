#ifndef __FILE_SERVER_STK5S_H__
#define __FILE_SERVER_STK5S_H__

#include "monapi/cmemoryinfo.h"

typedef long long int64_t;

int tek_decode(int siz, unsigned char *p, unsigned char *q);
int tek_checkformat(int siz, unsigned char *p);

#endif  // __FILE_SERVER_STK5S_H__
