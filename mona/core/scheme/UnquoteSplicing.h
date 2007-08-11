/*!
    \file  UnquoteSplicing.h
    \brief

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/08/14 update:$Date$
*/

#ifndef __UNQUAOTE_SPLICING_H__
#define __UNQUAOTE_SPLICING_H__

#include "scheme.h"

namespace monash {

class UnquoteSplicing : public Object
{
public:
    UnquoteSplicing(Object* object, uint32_t lineno = 0);
    virtual ~UnquoteSplicing();

public:
    virtual ::util::String toString();
    virtual int type() const;
    virtual Object* eval(Environment* env);
    virtual ::util::String typeString() const { return "unquote-splicing"; }
    virtual uint32_t lineno() const { return lineno_; }
    virtual bool eqv() const;
    virtual bool eq()  const;

protected:
    Object* object_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __UNQUAOTE_SPLICING_H__
