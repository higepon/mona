#ifndef _MACRO_
#define _MACRO_

#include "SExp.h"
#include "scheme.h"
#include "util/String.h"
#include "util/HashMap.h"
#include "util/Pair.h"
namespace monash {

#ifdef USE_BOEHM_GC
class Macro : public gc_cleanup
//class SExp : public gc
#else
class Macro
#endif
{
public:
    typedef ::util::Vector< ::util::Pair<SExp*, SExp*> > Patterns;

    Macro(const ::util::String& name);
    virtual ~Macro();
    void addPattern(SExp* pattern, SExp* definition);
    SExp* findPattern(SExp* pattern);

    static bool match(const ::util::String & macroName, const ::util::Strings & reservedWords, SExp* macro, SExp* target);
    SExp* match(const ::util::String& macroName, SExp* target);
    static ::util::String error;

    ::util::String name;
    Patterns patterns;
    ::util::Strings reservedWords;

private:
    static bool matchSExps(const ::util::String& macroName, const ::util::Strings& reservedWords, SExp* macro, SExp* target);
    static bool matchInternal(const ::util::String& macroName, const ::util::Strings& reservedWords, SExp* macro, SExp* target);
    static bool isReservedWord(SExp* sexp, const ::util::Strings& reservedWords);
    static bool checkReservedWord(SExp* macro, SExp* target, const ::util::Strings& reservedWords);
    static bool mustBeMacroName(SExp* sexp);
    static bool isMacroName(SExp* sexp, const ::util::String& macroName);
};

}; // namespace monash

#endif // _MACRO_
