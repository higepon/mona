#ifndef __NUMBER_H__
#define __NUMBER_H__

#include "Object.h"
#include "stdio.h"
#include "stdlib.h"

namespace monash {

class Number : public Object
{
public:
    Number(int value);
    virtual ~Number();

public:
    virtual std::string toString();
    virtual int type() const;
    virtual int value() const;
    virtual Object* eval(Environment* env);

protected:
    int value_;
    char buf_[64];
};

}; // namespace monash

#endif // __NUMBER_H__
