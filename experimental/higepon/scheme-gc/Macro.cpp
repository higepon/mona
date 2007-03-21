#include "Macro.h"

using namespace util;
using namespace monash;

String Macro::error;

Macro::Macro(const String& name) : name(name)
{
}

Macro::~Macro()
{
}

void Macro::addPattern(SExp* pattern, SExp* definition)
{
    patterns.add(new ::util::Pair<SExp*, SExp*>(pattern, definition));
}

SExp* Macro::findPattern(SExp* pattern)
{
    for (int i = 0; i < patterns.size(); i++)
    {
        if (patterns[i]->first == pattern) return patterns[i]->second;
    }
    return NULL;
}

bool Macro::checkReservedWord(SExp* macro, SExp* target, const Strings& reservedWords)
{
    if (!isReservedWord(macro, reservedWords)) return true;
    if (!target->isSymbol()) return false;
    return macro->text == target-> text;
}

// todo c++ cook book! FIX ME
bool Macro::isReservedWord(SExp* sexp, const Strings& reservedWords)
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

bool Macro::isMacroName(SExp* sexp, const String& macroName)
{
    return sexp->isSymbol() && sexp->text == macroName;
}

SExp* Macro::match(const String& macroName, SExp* target)
{
    for (int i = 0; i < patterns.size(); i++)
    {
        if (Macro::match(macroName, reservedWords, patterns[i]->first, target)) return patterns[i]->first;
    }
    return NULL;
}

bool Macro::matchSExps(const String& macroName, const Strings& reservedWords, SExp* macro, SExp* target)
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
bool Macro::match(const String& macroName, const Strings& reservedWords, SExp* macro, SExp* target)
{
    MACRO_TRACE_OUT("  macro match? %s :: %s\n", macro->toSExpString().data(), target->toSExpString().data());
    if (macro->isSExps() && target->isSExps())
    {
        if (macro->sexps.size() == target->sexps.size())
        {
            return matchSExps(macroName, reservedWords, macro, target);
        }
        else if (macro->sexps.size() < target->sexps.size())
        {
            if (macro->sexps.size() == 0)
            {
                MACRO_TRACE_OUT("    unmatch! macro->sexps.size() == 0 %s:%d\n", __FILE__, __LINE__);
                return false;
            }
            SExp* last = macro->sexps[macro->sexps.size() - 1];
            if (last->isMatchAllKeyword())
            {
                return matchSExps(macroName, reservedWords, macro, target);
            }
            else
            {
                MACRO_TRACE_OUT("    unmatch! %s:%d\n", __FILE__, __LINE__);
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
                    if (!match(macroName, reservedWords, m, t))
                    {
                        MACRO_TRACE_OUT("    unmatch! %s:%d\n", __FILE__, __LINE__);
                        return false;
                    }
                }
                MACRO_TRACE_OUT("    match %s:%d\n", __FILE__, __LINE__);
                return true;
            }
            else
            {
                MACRO_TRACE_OUT("    unmatch! %s:%d\n", __FILE__, __LINE__);
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
            MACRO_TRACE_OUT("    unmatch! %s:%d\n", __FILE__, __LINE__);
            return false;
        }
        if (!checkReservedWord(macro, target, reservedWords))
        {
            error = "reserved word " + macro->text + " unmatch";
            MACRO_TRACE_OUT("    unmatch! %s:%d\n", __FILE__, __LINE__);
            return false;
        }
        // add 2007/03/12 ugly
        if (macro->isSExps() && !target->isSExps())
        {
            MACRO_TRACE_OUT("    unmatch! %s:%d\n", __FILE__, __LINE__);
            return false;
        }
        MACRO_TRACE_OUT("    match %s:%d\n", __FILE__, __LINE__);
        return true;
    }
}

void Macro::extractBindings(SExp* m, SExp* n, BindMap& bindMap)
{
    if (m->isSymbol())
    {
        BindObject* b = new BindObject;
        b->sexp = n;
        MACRO_TRACE_OUT("     bind %s => %s\n", m->text.data(), n->toSExpString().data());
        bindMap.put(m->text.data(), b);
        return;
    }
    else if (m->isSExps() && n->isSExps())
    {
        int nLength = n->sexps.size();
        int mLength = m->sexps.size();
        for (int i = 0; i < m->sexps.size(); ++i)
        {
            if (i == nLength) return;
            SExp* mm = m->sexps[i];
            SExp* nn = n->sexps[i];
            if (mLength != nLength && mm->isMatchAllKeyword())
            {
                BindObject* b = new BindObject;;
                for (int j = i; j < nLength; ++j)
                {
                    b->sexps.add(n->sexps[j]);
                    MACRO_TRACE_OUT("     bind %s => %s\n", mm->text.data(), n->sexps[j]->toSExpString().data());
                }

                bindMap.put(mm->text.data(), b);
                return;
            }
            else
            {
               extractBindings(mm, nn, bindMap);
           }
        }
        return;
    }
    else
    {
        RAISE_ERROR(m->lineno, "macro exception \n%s\n%s", m->toString().data(), n->toString().data());
        return;
    }
}
