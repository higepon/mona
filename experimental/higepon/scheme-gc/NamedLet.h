#ifndef __NAMED_LET_H__
#define __NAMED_LET_H__

#include "Begin.h"
#include "Number.h"
#include "Lambda.h"

namespace monash {

class Application;

class NamedLet : public Object
{
public:
    NamedLet(Objects* body, Variables* variables, Objects* values, ::util::String name, uint32_t lineno = 0);
    virtual ~NamedLet();

public:
    virtual ::util::String toString();
    virtual int type() const;
    virtual uint32_t lineno() const { return lineno_; }
    virtual ::util::String typeString() const { return "let"; }
    virtual Object* eval(Environment* env);
    virtual Application* expand();

protected:
    Objects* body_;
    Variables* variables_;
    Objects* values_;
    ::util::String name_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __NAMED_LET_H__
