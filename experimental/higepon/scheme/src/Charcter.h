#ifndef __CHARCTER_H__
#define __CHARCTER_H__

#include "Object.h"

namespace monash {

class Charcter : public Object
{
public:
    Charcter(const std::string& value, uint32_t lineno = 0);
    virtual ~Charcter();

public:
    virtual std::string toString();
    virtual std::string toStringValue();
    virtual int type() const;
    virtual Object* eval(Environment* env);
    virtual uint32_t lineno() const { return lineno_; }
    virtual char value() { return value_; }
    virtual bool eqv(Object* o);
    virtual bool eq(Object* o);
    virtual bool gt(Object* o);
    virtual bool lt(Object* o);
    virtual bool ge(Object* o);
    virtual bool le(Object* o);

protected:
    char value_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __CHARCTER_H__
