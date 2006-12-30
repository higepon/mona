#ifndef _MACROUTILITY_
#define _MACROUTILITY_

#include <scheme.h>

namespace monash {

class MacroUtility
{
public:
    MacroUtility();
    virtual ~MacroUtility();

    static bool match(const std::string macroName, Node* macro, Node* target);
    static Node* macroToNode(const std::string text);
};

}; // namespace monash

#endif // _MACROUTILITY_
