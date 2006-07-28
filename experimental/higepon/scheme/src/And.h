#ifndef __AND_H__
#define __AND_H__

#include "scheme.h"

namespace monash {

class And : public Object
{
public:
    And(Objects* objects);
    virtual ~And();

public:
    virtual std::string toString();
    virtual int type() const;
    virtual Object* eval(Environment* env);

protected:
    Objects* objects_;
};

}; // namespace monash

#endif // __AND_H__
