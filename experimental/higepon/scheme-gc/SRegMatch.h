#ifndef __SREGMATCH_H__
#define __SREGMATCH_H__

#include "Object.h"
#include "procedures/Procedure.h"
#include <oniguruma.h>

namespace monash {

 class SRegexp;

class SRegMatch : public Object
{
public:
    SRegMatch(SRegexp* reg, OnigRegion* region, const ::util::String& text, uint32_t lineno = 0);
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
    virtual int matchStart(int i = 0);
    virtual int matchStart(const ::util::String& name);
    virtual int matchEnd(int i = 0);
    virtual int matchEnd(const ::util::String& name);
    virtual ::util::String matchSubString(int* result, int i = 0);
    virtual ::util::String matchSubString(int* result, const ::util::String& name);
    virtual ::util::String matchAfter(int* result, int i = 0);
    virtual ::util::String matchBefore(int* result, int i = 0);
    virtual int getNameIndex(const ::util::String& name);
    virtual int getNumMatches() const { return region_->num_regs; }
    virtual Object* apply(Objects* arguments, Environment* env);
protected:
    SRegexp* reg_;
    OnigRegion* region_;
    ::util::String text_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __SREGMATCH_H__
