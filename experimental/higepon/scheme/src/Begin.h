#ifndef __BEGIN_H__
#define __BEGIN_H__

#include "Object.h"
#include <vector>

namespace monash {

class Begin : public Object
{
public:
    Begin(Objects* actions);
    virtual ~Begin();

public:
    virtual std::string toString();
    virtual int type() const;
    virtual Objects* actions() const { return actions_;}

protected:
    Objects* actions_;
};

}; // namespace monash

#endif // __BEGIN_H__
