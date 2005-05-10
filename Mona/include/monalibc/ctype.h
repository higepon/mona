/*!
  \file   MlcCtype.h
  \brief  mona c character type

  Copyright (c) 2002-2004 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/02/25
  \author  shadow

  $Revision$
  $Date$
*/
#ifndef _MONA_LIB_CTYPE_
#define _MONA_LIB_CTYPE_

#ifdef __cplusplus
extern "C" {
#endif

int isalnum(int c);
int isalpha(int c);
int isascii(int c);
int isblank(int c);
int iscntrl(int c);
int isdigit(int c);
int isgraph(int c);
int islower(int c);
int isodigit(int c);
int isprint(int c);
int ispunct(int c);
int isspace(int c);
int isupper(int c);
int isxdigit(int c);

int toupper(int c);
int tolower(int c);

#ifdef __cplusplus
}
#endif

#endif
