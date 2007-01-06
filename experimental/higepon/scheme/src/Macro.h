#ifndef _MACRO_
#define _MACRO_

#include "Node.h"
#include <string>
#include <vector>
#include "scheme.h"

namespace monash {

class Macro
{
public:
    typedef std::map<Node*, Node*> Patterns;

    Macro(const std::string& name);
    virtual ~Macro();
    void addPattern(Node* pattern, Node* definition);

    static bool match(const std::string& macroName, const std::strings& reservedWords, Node* macro, Node* target);
    Node* match(const std::string& macroName, Node* target);
    static std::string error;

    std::string name;
    Patterns patterns;
    std::strings reservedWords;

private:
    static bool matchNodes(const std::string& macroName, const std::strings& reservedWords, Node* macro, Node* target);
    static bool matchInternal(const std::string& macroName, const std::strings& reservedWords, Node* macro, Node* target);
    static bool isReservedWord(Node* node, const std::strings& reservedWords);
    static bool checkReservedWord(Node* macro, Node* target, const std::strings& reservedWords);
    static bool mustBeMacroName(Node* node);
    static bool isMacroName(Node* node, const std::string& macroName);
};

}; // namespace monash

#endif // _MACRO_
