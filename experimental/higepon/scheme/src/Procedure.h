#ifndef __PROCEDURE_H__
#define __PROCEDURE_H__

#include "Lambda.h"
#include "Environment.h"

namespace monash {

class Lambda;

class Procedure
{
public:
    Procedure(Lambda* lambda, Environment* env);
    virtual ~Procedure();

    virtual std::string toString();
    virtual int type() const;
    Objects* body() { return body_; }
    Variables* parameters(){ return parameters_; }
    Environment* env() { return env_; }

protected:
    Objects* body_;
    Variables* parameters_;
    Environment* env_;
};

}; // namespace monash

#endif // __PROCEDURE_H__
