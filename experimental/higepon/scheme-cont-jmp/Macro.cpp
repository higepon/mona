#include "Macro.h"

using namespace monash;
using namespace std;

string Macro::error;

Macro::Macro(const string& name) : name(name)
{
}

Macro::~Macro()
{
}

void Macro::addPattern(SExp* pattern, SExp* definition)
{
    patterns.insert(pair<SExp*, SExp*>(pattern, definition));
}

bool Macro::checkReservedWord(SExp* macro, SExp* target, const strings& reservedWords)
{
    if (!isReservedWord(macro, reservedWords)) return true;
    if (!target->isSymbol()) return false;
    return macro->text == target-> text;
}

// todo c++ cook book! FIX ME
bool Macro::isReservedWord(SExp* sexp, const strings& reservedWords)
{
    if (!sexp->isSymbol()) return false;
    for (int i = 0; i < reservedWords.size(); i++)
    {
        if (reservedWords[i] == sexp->text) return true;
    }
    return false;
}

bool Macro::mustBeMacroName(SExp* sexp)
{
    return sexp->isSymbol() && sexp->text == "_";
}

bool Macro::isMacroName(SExp* sexp, const string& macroName)
{
    return sexp->isSymbol() && sexp->text == macroName;
}

SExp* Macro::match(const string& macroName, SExp* target)
{
    for (Macro::Patterns::const_iterator p = patterns.begin(); p != patterns.end(); ++p)
    {
        if (Macro::match(macroName, reservedWords, (*p).first, target)) return (*p).first;
    }
    return NULL;
}

bool Macro::matchSExps(const string& macroName, const strings& reservedWords, SExp* macro, SExp* target)
{
    for (int i = 0; i < macro->sexps.size(); i++)
    {
        SExp* m = macro->sexps.get(i);
        SExp* t = target->sexps.get(i);
        if (!match(macroName, reservedWords, m, t))
        {
            return false;
        }
    }
    return true;
}

// (_ a b ...)
// (and "hige" "huga" "hoge" "hoge")
bool Macro::match(const string& macroName, const strings& reservedWords, SExp* macro, SExp* target)
{
    if (macro->isSExps() && target->isSExps())
    {
        if (macro->sexps.size() == target->sexps.size())
        {
            return matchSExps(macroName, reservedWords, macro, target);
        }
        else if (macro->sexps.size() < target->sexps.size())
        {
            if (macro->sexps.size() == 0) return false;
            SExp* last = macro->sexps[macro->sexps.size() - 1];
            if (last->isMatchAllKeyword())
            {
                return matchSExps(macroName, reservedWords, macro, target);
            }
            else
            {
                return false;
            }
        }
        else
        {
            SExp* last = macro->sexps[macro->sexps.size() - 1];
            if (last->isMatchAllKeyword())
            {
                for (int i = 0; i < target->sexps.size(); i++)
                {
                    SExp* m = macro->sexps[i];
                    SExp* t = target->sexps[i];
                    if (!match(macroName, reservedWords, m, t)) return false;
                }
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    else
    {
        // Howerver "_" comes, macro name does not come.
        if (mustBeMacroName(macro) && !isMacroName(target, macroName))
        {
            error = "macro name unmatch";
            return false;
        }
        if (!checkReservedWord(macro, target, reservedWords))
        {
            error = "reserved word " + macro->text + " unmatch";
            return false;
        }
        return true;
    }
}
