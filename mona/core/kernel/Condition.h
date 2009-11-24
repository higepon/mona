/*!
    \file   Condition.h
    \brief  Condition variable.

    Copyright (c) 2002-2009 Higepon and the individuals listed on the ChangeLog entries.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2009/11/24
*/
#ifndef _MONA_CONDITION_
#define _MONA_CONDITION_

#include "KObject.h"

class Condition : public KObject {
  public:
    Condition() {}
    virtual ~Condition() {}

    int checkSecurity(Thread* thread)
    {
        return M_OK;
    }

    int getType() const
    {
        return CONDITION;
    }


};

#endif
