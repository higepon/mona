#ifndef _MACRO_
#define _MACRO_

#include "SExp.h"
#include <string>
#include <vector>
#include "scheme.h"

namespace monash {

class Macro
{
public:
    typedef std::map<SExp*, SExp*> Patterns;

    Macro(const std::string& name);
    virtual ~Macro();
    void addPattern(SExp* pattern, SExp* definition);

    static bool match(const std::string& macroName, const std::strings& reservedWords, SExp* macro, SExp* target);
    SExp* match(const std::string& macroName, SExp* target);
    static std::string error;

    std::string name;
    Patterns patterns;
    std::strings reservedWords;

private:
    static bool matchSExps(const std::string& macroName, const std::strings& reservedWords, SExp* macro, SExp* target);
    static bool matchInternal(const std::string& macroName, const std::strings& reservedWords, SExp* macro, SExp* target);
    static bool isReservedWord(SExp* sexp, const std::strings& reservedWords);
    static bool checkReservedWord(SExp* macro, SExp* target, const std::strings& reservedWords);
    static bool mustBeMacroName(SExp* sexp);
    static bool isMacroName(SExp* sexp, const std::string& macroName);
};

}; // namespace monash

#endif // _MACRO_
