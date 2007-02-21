#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "Object.h"
#include "Environment.h"

namespace monash {

class Application : public Object
{
public:
    Application(Object* function, Objects* arguments, uint32_t lineno = 0);
    virtual ~Application();

    virtual ::util::String toString();
    virtual int type() const;
    virtual Object* eval(Environment* env);
    virtual uint32_t lineno() const { return lineno_; }
    virtual ::util::String typeString() const { return "application"; }
    Object* function() { return function_; }
    Objects* arguments() { return arguments_; }

    //exp
protected:
    Object* function_;
    Objects* arguments_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __APPLICATION_H__
