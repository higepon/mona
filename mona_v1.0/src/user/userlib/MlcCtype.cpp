/*!
  \file   MlcCtype.cpp
  \brief  mona c character type

  Copyright (c) 2002-2004 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/02/25
  \author  shadow

  $Revision$
  $Date$
*/
#include <MlcCtype.h>

/*!
  \brief checks for an alphanumeric character

  \param c character to be checked
  \return  nonzero if c is alphanumeric, and zero if not
*/
int isalnum(int c){
  if(isascii(c)){
    return (isalpha(c) || isdigit(c));
  }

  return 0;
}

/*!
  \brief checks for an alphabetic character

  \param c character to be checked
  \return  nonzero if c is alphabetic, and zero if not
*/
int isalpha(int c){
  if(isascii(c)){
    return (isupper(c) || islower(c));
  }

  return 0;
}

/*!
  \brief checks for a 7-bit unsigned char

  \param c character to be checked
  \return  nonzero if c is ascii code, and zero if not
*/
int isascii(int c){
  return ((c & ~0x7f)?0:1);
}

/*!
  \brief checks for a blank character

  \param c character to be checked
  \return  nonzero if c is blank, and zero if not
*/
int isblank(int c){
  if(isascii(c)){
    return ((c == ' ') || (c == '\t'));
  }

  return 0;
}

/*!
  \brief checks for a control character

  \param c character to be checked
  \return  nonzero if c is control, and zero if not
*/
int iscntrl(int c){
  if(isascii(c)){
    return ((c < ' ') || (c == 0x7f)); 
  }

  return 0;
}

/*!
  \brief checks for a digit

  \param c character to be checked
  \return  nonzero if c is digit, and zero if not
*/
int isdigit(int c){
  if(isascii(c)){
    return ((c >= '0') && (c <= '9'));
  }

  return 0;
}

/*!
  \brief checks for any printable character except space

  \param c character to be checked
  \return  nonzero if c is printable except space, and zero if not
*/
int isgraph(int c){
  if(isascii(c)){
    return ((c > ' ') && (c < 0x7f));
  }

  return 0;
}

/*!
  \brief checks for a lower-case character

  \param c character to be checked
  \return  nonzero if c is lower, and zero if not
*/
int islower(int c){
  if(isascii(c)){
    return ((c >= 'a') && (c <= 'z'));
  }

  return 0;
}

/*!
  \brief checks for any printable character

  \param c character to be checked
  \return  nonzero if c is printable, and zero if not
*/
int isprint(int c){
  if(isascii(c)){
    return ((c >= ' ') && (c < 0x7f));
  }

  return 0;
}

/*!
  \brief checks for any printable character which is not a space or an alphanumeric character

  \param c character to be checked
  \return  nonzero if c is printable except space or alphanumeric, and zero if not
*/
int ispunct(int c){
  if(isascii(c)){
    return (isgraph(c) && !isalnum(c));
  }

  return 0;
}

/*!
  \brief checks for space characters

  \param c character to be checked
  \return  nonzero if c is space, and zero if not
*/
int isspace(int c){
  if(isascii(c)){
    return (((c >= 0x09) && (c <= 0x0d)) || (c == ' '));
  }

  return 0;
}

/*!
  \brief checks for an upper-case letter

  \param c character to be checked
  \return  nonzero if c is upper, and zero if not
*/
int isupper(int c){
  if(isascii(c)){
    return ((c >= 'A') && (c <= 'Z'));
  }

  return 0;
}

/*!
  \brief checks for a hexadecimal digits

  \param c character to be checked
  \return  nonzero if c is hexadecimal, and zero if not
*/
int isxdigit(int c){
  if(isascii(c)){
    return (isdigit(c) || ((c >= 'A') && (c <= 'F')) || ((c >= 'a') && (c <= 'f')));
  }

  return 0;
}

/*!
  \brief if can, c convert upper character

  \param c character to be converted
  \return  result of the conversion if can, and zero if not
*/
int toupper(int c){
  if(islower(c)) return (c + 'A' - 'a');

  return 0;
}

/*!
  \brief if can, c convert lower character

  \param c character to be converted
  \return  result of the conversion if can, and zero if not
*/
int tolower(int c){
  if(isupper(c)) return (c + 'a' - 'A');

  return 0;
}
