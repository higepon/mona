/*!
    \file  Definition.h
    \brief 

    Copyright (c) 2006-2007 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __DEFINITION_H__
#define __DEFINITION_H__

#include "Variable.h"
#include "Environment.h"
#include "Number.h"

namespace monash {

class Definition : public Object
{
public:
    Definition(Variable* variable, Object* value, uint32_t lineno = 0);
    virtual ~Definition();

public:
    virtual ::util::String toString();
    virtual int type() const { return Object::DEFINITION; }
    virtual Object* eval(Environment* env);
    virtual Variable* variable() const { return variable_; }
    virtual uint32_t lineno() const { return lineno_; }
    virtual ::util::String typeString() const { return "definition"; }
    virtual Object* value() const { return value_; }

protected:
    Variable* variable_;
    Object* value_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __DEFINITION_H__
