#ifndef __VARIABLE_H__
#define __VARIABLE_H__

#include "Object.h"

namespace monash {

class Variable : public Object
{
public:
    Variable(const std::string& name, uint32_t lineno = 0);
    virtual ~Variable();

public:
    virtual std::string toString();
    virtual std::string toStringValue();
    virtual int type() const;
    virtual Object* eval(Environment* env);
    virtual uint32_t lineno() const { return lineno_; }
    virtual std::string& name() { return name_; }

protected:
    std::string name_;
    uint32_t lineno_;
};

typedef util::Vector<Variable*> Variables;

}; // namespace monash

#endif // __VARIABLE_H__
