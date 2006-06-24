#ifndef __DEFINITION_H__
#define __DEFINITION_H__

#include "Variable.h"

namespace monash {

class Definition
{
public:
    Definition(Variable* variable, Object* value);
    virtual ~Definition();

public:
    virtual std::string toString();
    virtual int type() const;
    virtual Variable* variable() const { return variable_; }
    virtual Object* value() const { return value_; }

protected:
    Variable* variable_;
    Object* value_;
};

}; // namespace monash

#endif // __DEFINITION_H__
