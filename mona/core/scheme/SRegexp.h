/*!
    \file  SRegexp.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __SREGEXP_H__
#define __SREGEXP_H__

#include "Object.h"
#include "procedures/Procedure.h"
#include <oniguruma.h>

namespace monash {

class SRegexp : public Object
{
public:
    SRegexp(const ::util::String& value, bool caseFold = false, uint32_t lineno = 0);
    virtual ~SRegexp();

public:
    virtual ::util::String toString();
    virtual ::util::String toStringValue();
    virtual int type() const;
    virtual Object* eval(Environment* env);
    virtual ::util::String typeString() const { return "regexp"; }
    virtual uint32_t lineno() const { return lineno_; }
    virtual ::util::String pattern() { return pattern_; }
    virtual bool eqv(Object* o);
    virtual bool eq(Object* o);
    virtual bool equal(Object* o);
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
    virtual regex_t* getReg() { return reg_; }
    virtual bool isCaseFold() const { return caseFold_; }
protected:
    ::util::String pattern_;
    bool caseFold_;
    uint32_t lineno_;

    regex_t* reg_;
    OnigErrorInfo einfo_;
};

}; // namespace monash

#endif // __SREGEXP_H__
