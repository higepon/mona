#ifndef __OR_H__
#define __OR_H__

#include "scheme.h"

namespace monash {

class Or : public Object
{
public:
    Or(Objects* objects);
    virtual ~Or();

public:
    virtual std::string toString();
    virtual int type() const;
    virtual Object* eval(Environment* env);

protected:
    Objects* objects_;
};

}; // namespace monash

#endif // __OR_H__
