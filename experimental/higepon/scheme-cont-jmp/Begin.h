#ifndef __BEGIN_H__
#define __BEGIN_H__

#include "Object.h"

namespace monash {

class Begin : public Object
{
public:
    Begin(Objects* actions, uint32_t lineno = 0);
    virtual ~Begin();

public:
    virtual std::string toString();
    virtual int type() const;
    virtual uint32_t lineno() const { return lineno_; }
    virtual Object* eval(Environment* env);
    virtual Objects* actions() const { return actions_;}
    virtual bool eqv() const;
    virtual bool eq()  const;
    virtual Object* getContinuation(Object* object);

protected:
    Objects* actions_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __BEGIN_H__
