#ifndef __QUOTE_H__
#define __QUOTE_H__

#include "Object.h"
#include "scheme.h"

namespace monash {

class Quote : public Object
{
public:
    Quote(SExp* sexp, uint32_t lineno = 0);
    virtual ~Quote();

public:
    virtual ::util::String toString();
    virtual ::util::String toStringValue();
    virtual int type() const;
    virtual Object* eval(Environment* env);
    virtual uint32_t lineno() const { return lineno_; }
    virtual ::util::String text() const { return value_;}
    virtual SExp* sexp() { return sexp_;}
    virtual bool eqv(Object* o);
    virtual bool eq(Object* o);

    virtual Quote* car();
    virtual Quote* cdr();

protected:
    SExp* sexp_;
    ::util::String value_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __QUOTE_H__
