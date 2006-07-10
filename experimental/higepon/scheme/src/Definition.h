#ifndef __DEFINITION_H__
#define __DEFINITION_H__

#include "Variable.h"
#include "Environment.h"
#include "Quote.h"

namespace monash {

class Definition : public Object
{
public:
    Definition(Variable* variable, Object* value);
    virtual ~Definition();

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

#endif // __DEFINITION_H__
