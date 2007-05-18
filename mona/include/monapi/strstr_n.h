/*!
    \file  strstr_n.h
    \brief standard library

    standard library

    Copyright (c) 2007 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2007/05/18 update:$Date$
*/
#ifndef _MONAPI_STRSTR_N_
#define _MONAPI_STRSTR_N_

#include<sys/types.h>

namespace MonAPI {
    char* strstr_n(const char* str1, const char* str2, int size);
};

#endif // _MONAPI_STRSTR_N_
