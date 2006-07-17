#ifndef __LAMBDA_H__
#define __LAMBDA_H__

#include "scheme.h"
#include "Object.h"

namespace monash {

class Lambda : public Object
{
public:
    Lambda(Objects* body, Variables* parameters);
    virtual ~Lambda();

    virtual std::string toString();
    virtual int type() const;
    virtual Object* eval(Environment* env);

    Objects* body() const { return body_; }
    Variables* parameters() { return parameters_;}

protected:
    Objects* body_;
    Variables* parameters_;
};

}; // namespace monash

#endif // __LAMBDA_H__
