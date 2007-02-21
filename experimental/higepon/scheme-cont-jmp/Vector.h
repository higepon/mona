#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "Object.h"

namespace monash {

class Vector : public Object
{
public:
    Vector(uint32_t size, uint32_t lineno = 0);
    Vector(uint32_t size, Object* o, uint32_t lineno = 0);
    Vector(Objects* values, uint32_t lineno = 0);
    virtual ~Vector();

public:
    virtual ::util::String toString();
    virtual ::util::String toStringValue();
    virtual int type() const;
    virtual Object* eval(Environment* env);
    virtual ::util::String typeString() const { return "vector"; }
    virtual uint32_t lineno() const { return lineno_; }
    virtual bool eqv(Object* o);
    virtual bool eq(Object* o);
    virtual uint32_t size() const { return size_; }
    virtual Object* get(uint32_t index);
    virtual bool set(uint32_t index, Object* o);

protected:
    Objects* values_;
    const uint32_t size_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __VECTOR_H__
