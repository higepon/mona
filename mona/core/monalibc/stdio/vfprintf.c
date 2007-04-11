/*!
  \file   vfprintf.c
  \brief  vfprintf

  Copyright (c) 2002-2004 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/03/13
  \author  shadow

  $Revision: 3290 $
  $Date: 2006-06-19 00:27:38 +0900 (月, 19  6月 2006) $
*/

#include <monalibc/stdio.h>
#include <monapi/string.h>

/*!
  \brief 

  \param stream file stream printed characters
  \param format specifies how subsequent arguments
  \param ap     a variable number of arguments
  \return  the number of characters printed
 */
int vfprintf(FILE *stream, const char *format, va_list ap)
{
	char buf[5096];

	vsprintf(buf, format, ap);
	printf(buf);

	return strlen(buf);
}

