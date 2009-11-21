/*!
    \file  Variable.h
    \brief 

    Copyright (c) 2006-2007 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __VARIABLE_H__
#define __VARIABLE_H__

#include "Object.h"

namespace monash {

class Variable : public Object
{
public:
    Variable(const ::util::String& name, uint32_t lineno = 0);
    virtual ~Variable();

public:
    virtual ::util::String toString();
    virtual ::util::String toStringValue();
    virtual int type() const;
    virtual Object* eval(Environment* env);
    virtual uint32_t lineno() const { return lineno_; }
    virtual ::util::String typeString() const { return "variable"; }
    virtual ::util::String& name() { return name_; }

protected:
    ::util::String name_;
    uint32_t lineno_;
};

typedef ::util::Vector<Variable*> Variables;

}; // namespace monash

#endif // __VARIABLE_H__
