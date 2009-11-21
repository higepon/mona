/*!
    \file  Let.h
    \brief 

    Copyright (c) 2006-2007 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __LET_H__
#define __LET_H__

#include "Begin.h"
#include "Number.h"
#include "Lambda.h"

namespace monash {

class Application;

class Let : public Object
{
public:
    Let(Objects* body, Variables* variables, Cons* values, uint32_t lineno = 0);
    virtual ~Let();

public:
    virtual ::util::String toString();
    virtual int type() const;
    virtual uint32_t lineno() const { return lineno_; }
    virtual ::util::String typeString() const { return "let"; }
    virtual Object* eval(Environment* env);
    virtual Application* expand();

protected:
    Objects* body_;
    Variables* variables_;
    Cons* values_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __LET_H__
