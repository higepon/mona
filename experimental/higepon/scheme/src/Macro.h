#ifndef _MACRO_
#define _MACRO_

#include <scheme.h>

// namespace std {
//     typedef vector<string> strings;
// };

namespace monash {

class Macro
{
public:
    Macro();
    virtual ~Macro();

    static bool match(const std::string& macroName, const std::strings& reservedWords, Node* macro, Node* target);
    static Node* toNode(const std::string& text);
    static std::string error;
private:
    static bool matchInternal(const std::string& macroName, const std::strings& reservedWords, Node* macro, Node* target);
    static bool isMatchAllKeyword(Node* node);
    static bool isReservedWord(Node* node, const std::strings& reservedWords);
    static bool checkReservedWord(Node* macro, Node* target, const std::strings& reservedWords);
    static bool mustBeMacroName(Node* node);
    static bool isMacroName(Node* node, const std::string& macroName);
};

}; // namespace monash

#endif // _MACRO_
