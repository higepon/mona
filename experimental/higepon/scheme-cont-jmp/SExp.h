#ifndef __SEXP_H__
#define __SEXP_H__

#include <string>
#include <stdio.h>
#include <map>
#include <stdio.h>
#include <util/Vector.h>
#include <util/HashMap.h>
#include "Tokenizer.h"
#include "Error.h"
#include "Parser.h"

::util::String load(const char* file);
namespace monash {

class SExp;
typedef ::util::Vector<SExp*> SExps;

#define N(n)         sexp->sexps[n]
#define NN(i, j)     sexp->sexps[i]->sexps[j]
#define NNN(i, j, k) sexp->sexps[i]->sexps[j]->sexps[k]
#define L()          sexp->sexps.size()
#define LL(n)        sexp->sexps[n]->sexps.size()


class BindObject;
typedef ::util::HashMap<BindObject*> BindMap;

class BindObject
{
public:
    BindObject() : sexp(NULL) {}
public:
    SExp* sexp;
    SExps sexps;
};


class SExp
{
public:
    SExp(int type) : type(type), lineno(0) {}
    ~SExp() {}

    ::util::String toString();
    ::util::String toSExpString();
    void print(int depth = 0);
    bool equals(SExp* sexp);
    SExp* clone();

    bool isSExps()  const { return type == SEXPS; }
    bool isNumber() const { return type == NUMBER; }
    bool isSymbol() const { return type == SYMBOL; }
    bool isString() const { return type == STRING; }
    bool isQuote()  const { return type == QUOTE; }
    bool isChar()  const { return type == CHAR; }
    bool isMatchAllKeyword() { return isSymbol() && !text.include("..."); }

    static SExp* fromString(const ::util::String& text);
    static void extractBindings(SExp* m, SExp* n, BindMap& bindMap);

    int foreachSExp(SExp* root, bool (SExp::*match)() const, int (SExp::*func)(SExp* root, SExp* sexp));
    int foreachSExps(SExp* root, int (SExp::*f)(SExp*, SExp*));
#if 0
    int execLoadSyntax(SExp* root, SExp* sexp);
    int execLoadSyntaxes();
#endif
    SExps sexps;
    ::util::String text;
    int value;
    int type;
    uint32_t lineno;

    enum
    {
        SEXPS,
        NUMBER,
        SYMBOL,
        STRING,
        QUOTE,
        CHAR,
    };

private:
    static bool equalsInternal(SExp* m, SExp* n);
    void toStringInternal(uint32_t depth, ::util::String& s);
    void toSExpStringInternal(::util::String& s);
    ::util::String typeToString();
    ::util::String typeToRawString();
};

}; // namespace monash

#endif // __SEXP_H__
