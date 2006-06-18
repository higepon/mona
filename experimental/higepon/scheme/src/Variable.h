#ifndef __VARIABLE_H__
#define __VARIABLE_H__

#include "Object.h"

namespace monash {

class Variable : Object
{
public:
    Variable(Object* object);
    virtual ~Variable();

public:
    virtual std::string toString();
    virtual int type() const;

protected:
    Object* object_;
};

}; // namespace monash

#endif // __VARIABLE_H__
