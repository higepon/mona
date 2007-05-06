/*!
    \file  uitoa.h
    \brief standard library

    standard library

    Copyright (c) 2007 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2007/05/06 update:$Date$
*/
#ifndef _MONAPI_UITOA_
#define _MONAPI_UITOA_

#include <sys/types.h>
#include <monapi/get_digits.h>
#include <monapi/power.h>

namespace MonAPI {
    char* uitoa(char* buf, uint32_t n);
};

#endif // _MONAPI_UITOA_
