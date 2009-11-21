/*!
    \file  Assert.h
    \brief 

    Copyright (c) 2006-2007 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __UTIL_ASSERT_H__
#define __UTIL_ASSERT_H__

namespace monash {
namespace util {

#define ASSERT_UTIL(condition) {if (!(condition)) {printf("ASSERT failure %s:%d: %s\n", __FILE__, __LINE__, #condition); }}

};
};
#endif // __UTIL_ASSERT_H__
