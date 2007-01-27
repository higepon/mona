#ifndef __PROCEDURE_H__
#define __PROCEDURE_H__

#include "Lambda.h"
#include "Environment.h"

namespace monash {

class Lambda;

class Procedure : public Object
{
public:
    Procedure(Lambda* lambda, Environment* env, uint32_t lineno = 0);
    virtual ~Procedure();

    virtual std::string toString();
    virtual int type() const;
    virtual uint32_t lineno() const { return lineno_; }
    virtual Object* eval(Environment* env);
    Objects* body() { return body_; }
    Variables* parameters(){ return parameters_; }
    Environment* env() { return env_; }

    virtual bool eqv() const;
    virtual bool eq()  const;


protected:
    Objects* body_;
    Variables* parameters_;
    Environment* env_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __PROCEDURE_H__
