#ifndef __SREGMATCH_H__
#define __SREGMATCH_H__

#include "Object.h"
#include "procedures/Procedure.h"
#include <oniguruma.h>

namespace monash {

class SRegMatch : public Object
{
public:
    SRegMatch(OnigRegion* region, uint32_t lineno = 0);
    virtual ~SRegMatch();

public:
    virtual ::util::String toString();
    virtual ::util::String toStringValue();
    virtual int type() const;
    virtual Object* eval(Environment* env);
    virtual ::util::String typeString() const { return "regmatch"; }
    virtual uint32_t lineno() const { return lineno_; }
    virtual bool eqv(Object* o);
    virtual bool eq(Object* o);
    virtual bool equal(Object* o);
    virtual int matchStart();
protected:
    OnigRegion* region_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __SREGMATCH_H__
