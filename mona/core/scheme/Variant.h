/*!
    \file  Variant.h
    \brief 

    Copyright (c) 2006-2007 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __VARIANT__
#define __VARIANT__

#include "Object.h"
//#include "scheme.h"

namespace monash {

class Variant : public Object
{
public:
    Variant(void* data, uint32_t lineno = 0);
    virtual ~Variant();

public:
    virtual ::util::String toString();
    virtual ::util::String toStringValue();
    virtual int type() const;
    virtual Object* eval(Environment* env);
    virtual uint32_t lineno() const { return lineno_; }
    virtual ::util::String typeString() const { return "#<variant>"; }
    virtual void* data() { return data_; }
    virtual bool eqv(Object* o);
    virtual bool eq(Object* o);

protected:
    void* data_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __VARIANT__
