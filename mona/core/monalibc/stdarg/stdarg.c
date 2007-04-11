/*!
  \file   MlcStdarg.cpp
  \brief  mona c standard argument

  Copyright (c) 2002-2004 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/03/13
  \author  shadow

  $Revision: 3290 $
  $Date: 2006-06-19 00:27:38 +0900 (月, 19  6月 2006) $
*/

#include <monalibc.h>
#include <monalibc/assert.h>
#include <monapi/string.h>
#include <monapi/syscall.h>

/*!
  
 */
int vsnprintf(char *str, size_t size, const char* format, va_list ap)
{
	int len;
	len = vsprintf(str, format, ap);
	assert(len <= (int)size);

	return len;
}

