#ifndef __PAIR_H__
#define __PAIR_H__

#include "Object.h"
#include "SExp.h"
#include "Number.h"
#include "Variable.h"
#include "RiteralConstant.h"

namespace monash {

class Pair : public Object
{
public:
    Pair(Object* first, Object* second, uint32_t lineno = 0);
    virtual ~Pair();

    Object* getCar() {return car_;}
    Object* getCdr() {return cdr_;}
    void setCar(Object* o);
    void setCdr(Object* o);
    SExp* toSExp();
    SExp* objectToSExp(Object* o);

public:
    virtual ::util::String toString();
    virtual ::util::String toStringValue();
    virtual int type() const;
    virtual uint32_t lineno() const { return lineno_; }
    virtual ::util::String typeString() const { return "pair"; }
    virtual Object* eval(Environment* env);
    virtual bool eqv(Object* o);
    virtual bool eq(Object* o);

protected:
    Object* car_;
    Object* cdr_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __STRING_H__
