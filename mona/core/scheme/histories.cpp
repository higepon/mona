/*!
    \file   histories.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#ifdef MONA
#include <monapi.h>
#include "MonaTerminal.h"
#include <stdint.h>
#include <limits.h>
#include <string>


using namespace monash;

void MonaTerminal::initHistories()
{
    MonAPI::scoped_ptr<MonAPI::SharedMemory> shm(monapi_file_read_all(HISTORY_FILE));
    if (shm.get() == NULL) {
        return;
    }
    std::string history;
    for (int i = 0; i < shm->size(); i++) {
        if (shm->data()[i] == '\n') {
            histories_.add(history.c_str());
            history.clear();
        } else {
            history += shm->data()[i];
        }
    }
}
#endif // MONA
