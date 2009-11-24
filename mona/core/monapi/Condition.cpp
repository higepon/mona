/*!
    \file  Condition.cpp
    \brief class Condition

    Copyright (c) 2002-2009 Higepon and the individuals listed on the ChangeLog entries.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2004/01/12 update:$Date$
*/
#include <monapi.h>

namespace MonAPI {

Condition::Condition()
{
}

Condition::~Condition()
{

}

intptr_t Condition::waitWithTimeout(Mutex* mutex, intptr_t timeoutMsec)
{
}

intptr_t Condition::wait(Mutex* mutex)
{


}

intptr_t Condition::notifyAll()
{
}

}
