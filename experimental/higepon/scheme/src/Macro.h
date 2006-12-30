#ifndef _MACRO_
#define _MACRO_

#include <scheme.h>

namespace monash {

class Macro
{
public:
    Macro();
    virtual ~Macro();

    static bool match(const std::string& macroName, Node* macro, Node* target);
    static Node* toNode(const std::string& text);
};

}; // namespace monash

#endif // _MACRO_
