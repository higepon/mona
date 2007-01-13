#ifndef __STRING_H__
#define __STRING_H__

#include "Object.h"

namespace monash {

class String : public Object
{
public:
    String(const std::string& value, uint32_t lineno = 0);
    virtual ~String();

public:
    virtual std::string toString();
    virtual std::string toStringValue();
    virtual int type() const;
    virtual Object* eval(Environment* env);
    virtual uint32_t lineno() const { return lineno_; }
    virtual std::string value() { return value_; }
    virtual bool eqv(Object* o);
    virtual bool eq(Object* o);
    virtual bool equal(Object* o);

protected:
    std::string value_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __STRING_H__
