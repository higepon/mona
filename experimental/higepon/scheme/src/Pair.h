#ifndef __PAIR_H__
#define __PAIR_H__

#include "Object.h"

namespace monash {

class Pair : public Object
{
public:
    Pair(Object* first, Object* second);
    virtual ~Pair();

    Object* first() {return first_;}
    Object* second() {return second_;}

public:
    virtual std::string toString();
    virtual int type() const;
    virtual Object* eval(Environment* env);

protected:
    Object* first_;
    Object* second_;
};

}; // namespace monash

#endif // __STRING_H__
