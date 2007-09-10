/*!
    \file  Environment.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __ENVIRONMENT_H__
#define __ENVIRONMENT_H__

#include <stdio.h>
#include "Frame.h"

namespace monash {

class MacroFilter;
class Translator;

class Environment : public Object
{
protected:
    typedef ::util::Vector<Frame*> Frames;
    Frames* frames_;
    uint32_t lineno_;

public:
    Environment(uint32_t lineno = 0);
    virtual ~Environment();
    Environment* clone();
    Object* lookupVariableValue(Variable* variable);
    void setVaribale(Variable* variable, Object* value);
    void defineVariable(Variable* variable, Object* value);
    Frames* frames() {return frames_;}
    void extend(Variables* variables, Objects* objects);
    ::util::String toString();
    virtual int type() const { return Object::ENVIRONMENT; }
    virtual uint32_t lineno() const { return lineno_; }
    virtual ::util::String typeString() const { return "environment"; }
    virtual Object* eval(Environment* env);
};

}; // namespace monash

#endif // __ENVIRONMENT_H__
