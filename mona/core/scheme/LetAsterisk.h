/*!
    \file  LetAsterisk.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __LET_ASTERISK_H__
#define __LET_ASTERISK_H__

#include "Begin.h"
#include "Number.h"
#include "Lambda.h"

namespace monash {

class LetAsterisk : public Object
{
public:
    LetAsterisk(Objects* body, Variables* variables, Cons* values, uint32_t lineno = 0);
    virtual ~LetAsterisk();

public:
    virtual ::util::String toString();
    virtual int type() const;
    virtual ::util::String typeString() const { return "let*"; }
    virtual uint32_t lineno() const { return lineno_; }
    virtual Object* eval(Environment* env);
    virtual Object* expand();

protected:
    Objects* expandInternal(int variablesIndex, int valuesIndex);
    Objects* body_;
    Variables* variables_;
    Cons* values_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __LET_ASTERISK_H__
