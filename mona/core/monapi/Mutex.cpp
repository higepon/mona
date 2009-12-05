/*!
    \file  Mutex.cpp
    \brief class kMutex

    Copyright (c) 2002-2009 Higepon and the individuals listed on the ChangeLog entries.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2004/01/12 update:$Date$
*/
#include <monapi.h>

namespace MonAPI {

// Placed on cpp file for breaking the build dependencies.
intptr_t Mutex::destroy()
{
    if (destroyed_) {
        return M_OK;
    }
    destroyed_ = true;
    return syscall_mutex_destroy(mutexId_);
}

}
