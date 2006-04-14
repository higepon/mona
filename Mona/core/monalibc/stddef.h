/*!
  \file   stddef.h
  \brief  monalibcc standard argument Header

  Copyright (c) 2002-2005 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2005/05/04
  \author  shadow

  $Revision$
  $Date$
*/
#ifndef _MONA_LIB_STDDEF_
#define _MONA_LIB_STDDEF_

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef signed int ptrdiff_t;

#define offsetof(s_name,m_name) (size_t)&(((s_name*)0)->m_name)

#ifdef __cplusplus
}
#endif

#endif /* _MONA_LIB_STDDEF_ */
