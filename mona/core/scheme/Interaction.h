/*!
    \file  Interaction.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __INTERACTION_H__
#define __INTERACTION_H__

#include <util/String.h>
#include "Environment.h"
#include "SString.h"
#include "scheme.h"

namespace monash {

class Interaction
{
public:
    Interaction(Environment* env);
    virtual ~Interaction();

    bool onInput(const ::util::String& line);
    void reset();
    void showPrompt();

protected:
    ::util::String input_;
    Environment* env_;
};

}; // namespace monash

#endif // __INTERACTION_H__
