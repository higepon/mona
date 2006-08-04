#ifndef __ENVIRONMENT_H__
#define __ENVIRONMENT_H__

#include <vector>
#include <stdio.h>
#include "Frame.h"

namespace monash {

class Environment
{
protected:
    typedef std::vector<Frame*> Frames;
    Frames* frames_;

public:
    Environment();
    virtual ~Environment();
    Environment* clone();
    Object* lookupVariableValue(Variable* variable);
    void setVaribale(Variable* variable, Object* value);
    void defineVariable(Variable* variable, Object* value);
    Frames* frames() {return frames_;}
    void extend(Variables* variables, Objects* objects);
    std::string toString();
};

}; // namespace monash

#endif // __ENVIRONMENT_H__
