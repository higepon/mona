#ifndef __INTERACTION_H__
#define __INTERACTION_H__

#include <util/String.h>
#include "Environment.h"
#include "SString.h"
#include "scheme.h"

namespace monash {

class Interaction
{
public:
    Interaction(Environment* env);
    virtual ~Interaction();

    void onInput(const ::util::String& line);
    void reset();
    void showPrompt();

protected:
    ::util::String input_;
    Environment* env_;
};

}; // namespace monash

#endif // __INTERACTION_H__
