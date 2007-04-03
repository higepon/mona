#ifndef __NUMBER_H__
#define __NUMBER_H__

#include "Object.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef MONA
#define snprintf(a, b, ...) sprintf(a, __VA_ARGS__)
#endif

namespace monash {

class Number : public Object
{
public:
    Number(int value, uint32_t lineno = 0);
    virtual ~Number();

public:
    virtual ::util::String toString();
    virtual ::util::String toStringValue();
    bool eqv(Object* o);
    bool eq(Object* o);

    virtual int type() const;
    virtual ::util::String typeString() const { return "number"; }
    virtual uint32_t lineno() const { return lineno_; }
    virtual int value() const;
    virtual Object* eval(Environment* env);

protected:
    int value_;
    uint32_t lineno_;
    char buf_[64];
};

}; // namespace monash

#endif // __NUMBER_H__
