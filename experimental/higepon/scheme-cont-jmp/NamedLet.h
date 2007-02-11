#ifndef __NAMED_LET_H__
#define __NAMED_LET_H__

#include "Begin.h"
#include "Number.h"
#include "Lambda.h"
#include <vector>
#include <map>

namespace monash {

class Application;

class NamedLet : public Object
{
public:
    NamedLet(Objects* body, Variables* variables, Objects* values, std::string name, uint32_t lineno = 0);
    virtual ~NamedLet();

public:
    virtual std::string toString();
    virtual int type() const;
    virtual uint32_t lineno() const { return lineno_; }
    virtual Object* eval(Environment* env);
    virtual Application* expand();

protected:
    Objects* body_;
    Variables* variables_;
    Objects* values_;
    std::string name_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __NAMED_LET_H__
