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
    MacroFilter& filter_;
    Translator& translator_;
    uint32_t lineno_;

public:
    Environment(MacroFilter& filter, Translator& translator, uint32_t lineno = 0);
    virtual ~Environment();
    Environment* clone();
    Object* lookupVariableValue(Variable* variable);
    void setVaribale(Variable* variable, Object* value);
    void defineVariable(Variable* variable, Object* value);
    Frames* frames() {return frames_;}
    void extend(Variables* variables, Objects* objects);
    ::util::String toString();
    MacroFilter& macroFilter() { return filter_; }
    Translator& translator() { return translator_; }
    virtual int type() const;
    virtual uint32_t lineno() const { return lineno_; }
    virtual Object* eval(Environment* env);
};

}; // namespace monash

#endif // __ENVIRONMENT_H__
