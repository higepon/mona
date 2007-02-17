#ifndef __CONTINUATION_H__
#define __CONTINUATION_H__

#include "Object.h"
extern "C" {
#include "libcont/cont.h"
}
#include "procedures/Procedure.h"

namespace monash {

class Continuation : public Object
{
public:
    Continuation(uint32_t lineno = 0);
    virtual ~Continuation();

public:
    virtual ::util::String toString();
    virtual ::util::String toStringValue();
    virtual int type() const;
    virtual Object* eval(Environment* env);
    virtual uint32_t lineno() const { return lineno_; }
    virtual bool eqv(Object* o);
    virtual bool eq(Object* o);
    virtual bool equal(Object* o);
    virtual Object* apply(Objects* arguments, Environment* env);

    Cont cont;
    Object* callArugument;
protected:

    uint32_t lineno_;
};

}; // namespace monash

#endif // __CONTINUATION_H__
