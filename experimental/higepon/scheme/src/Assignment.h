#ifndef __ASSIGNMENT_H__
#define __ASSIGNMENT_H__

#include "Variable.h"

namespace monash {

class Assignment
{
public:
    Assignment(Variable* variable, Object* value);
    virtual ~Assignment();

public:
    virtual std::string toString();
    virtual int type() const;

protected:
    Variable* variable_;
    Object* value_;
};

}; // namespace monash

#endif // __ASSIGNMENT_H__
