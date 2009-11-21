/*!
    \file  Assert.h
    \brief 

    Copyright (c) 2006-2007 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __ASSERT_H__
#define __ASSERT_H__

#define SCM_ASSERT(condition) {if (!(condition)) {printf("ASSERT failure %s:%d: %s\n", __FILE__, __LINE__, #condition);}}

#endif // __ASSERT_H__
