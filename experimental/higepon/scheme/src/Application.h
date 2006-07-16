#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "Object.h"
#include "Environment.h"

namespace monash {

class Application : public Object
{
public:
    Application(Object* function, Objects* arguments);
    virtual ~Application();

    virtual std::string toString();
    virtual int type() const;
    Object* function() { return function_; }
    Objects* arguments() { return arguments_; }

protected:
    Object* function_;
    Objects* arguments_;
};

}; // namespace monash

#endif // __APPLICATION_H__
