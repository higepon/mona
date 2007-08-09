/*!
    \file  Assignment.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __ASSIGNMENT_H__
#define __ASSIGNMENT_H__

#include "Variable.h"
#include "Environment.h"
#include "Number.h"

namespace monash {


class Assignment : public Object
{
public:
    Assignment(Variable* variable, Object* value);
    virtual ~Assignment();

public:
    virtual ::util::String toString();
    virtual int type() const;
    virtual Object* eval(Environment* env);
    virtual Variable* variable() const { return variable_; }
    virtual Object* value() const { return value_; }

protected:
    Variable* variable_;
    Object* value_;
};

}; // namespace monash

#endif // __ASSIGNMENT_H__
