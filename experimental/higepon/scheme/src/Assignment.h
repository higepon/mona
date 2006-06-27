#ifndef __ASSIGNMENT_H__
#define __ASSIGNMENT_H__

#include "Variable.h"
#include "Environment.h"
#include "Quote.h"

namespace monash {

class Assignment
{
public:
    Assignment(Variable* variable, Object* value);
    virtual ~Assignment();

public:
    virtual std::string toString();
    virtual int type() const;
    virtual Variable* variable() const { return variable_; }
    virtual Object* value() const { return value_; }
    virtual Object* eval(Environment* env);

protected:
    Variable* variable_;
    Object* value_;
};

}; // namespace monash

#endif // __ASSIGNMENT_H__
