#ifndef __SSTRING_H__
#define __SSTRING_H__

#include "Object.h"
#include "Charcter.h"

namespace monash {

class SString : public Object
{
public:
    SString(const ::util::String& value, uint32_t lineno = 0);
    SString(uint32_t length, uint32_t lineno = 0);
    SString(uint32_t length, char c, uint32_t lineno = 0);
    virtual ~SString();

public:
    virtual ::util::String toString();
    virtual ::util::String toStringValue();
    virtual int type() const;
    virtual Object* eval(Environment* env);
    virtual uint32_t lineno() const { return lineno_; }
    virtual ::util::String value() { return value_; }
    virtual bool eqv(Object* o);
    virtual bool eq(Object* o);
    virtual bool equal(Object* o);
    virtual Charcter* get(uint32_t index);
    virtual bool set(uint32_t index, Charcter* c);
    SString* clone();
protected:
    ::util::String value_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __STRING_H__
