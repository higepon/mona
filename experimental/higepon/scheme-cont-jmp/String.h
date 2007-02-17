#ifndef __STRING_H__
#define __STRING_H__

#include "Object.h"
#include "Charcter.h"

namespace monash {

class String : public Object
{
public:
    String(const ::util::String& value, uint32_t lineno = 0);
    String(uint32_t length, uint32_t lineno = 0);
    String(uint32_t length, char c, uint32_t lineno = 0);
    virtual ~String();

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
    virtual Charcter* get(int index);
    virtual bool set(int index, Charcter* c);
    String* clone();
protected:
    ::util::String value_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __STRING_H__
