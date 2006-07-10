#ifndef __VARIABLE_H__
#define __VARIABLE_H__

#include "Object.h"
#include <vector>

namespace monash {

class Variable : public Object
{
public:
    Variable(const std::string& name);
    virtual ~Variable();

public:
    virtual std::string toString();
    virtual int type() const;

protected:
    std::string name_;
};

typedef std::vector<Variable*> Variables;

}; // namespace monash

#endif // __VARIABLE_H__
