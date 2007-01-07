#ifndef __PAIR_H__
#define __PAIR_H__

#include "Object.h"

namespace monash {

class Pair : public Object
{
public:
    Pair(Object* first, Object* second, uint32_t lineno = 0);
    virtual ~Pair();

    Object* first() {return first_;}
    Object* second() {return second_;}

public:
    virtual std::string toString();
    virtual int type() const;
    virtual uint32_t lineno() const { return lineno_; }
    virtual Object* eval(Environment* env);

protected:
    Object* first_;
    Object* second_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __STRING_H__
