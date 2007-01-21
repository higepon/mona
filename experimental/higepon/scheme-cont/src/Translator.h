#ifndef __TRANSLATOR_H__
#define __TRANSLATOR_H__

#include "SExp.h"
#include "Object.h"
#include "Procedure.h"
#include "Number.h"
#include "Charcter.h"
#include "String.h"
#include "Quote.h"
#include "SpecialIf.h"
#include "Assignment.h"
#include "Begin.h"
#include "Definition.h"
#include "Application.h"
#include "Cond.h"
#include "Let.h"
#include "LetAsterisk.h"
#include "Macro.h"
#include "SExp.h"

namespace monash {

class Translator
{
public:
    Translator();
    virtual ~Translator();

public:
    enum
    {
        SYNTAX_ERROR,
        SUCCESS
    };

    void step1(SExp* root);
    int translate(SExp** sexp, Object** object);
    int translateDefineSyntax(SExp* sexp);
    SExp* expandMacroIfMatch(const std::string& name, SExp** sexp);
private:
    int translatePrimitive(SExp* sexp, Object** object);
    int translateDefinition(SExp* sexp, Object** object);

    int translateIf(SExp* sexp, Object** object);
    int translateAnd(SExp* sexp, Object** object);
    int translateOr(SExp* sexp, Object** object);
    int translateCond(SExp* sexp, Object** object);
    int translateBegin(SExp* sexp, Object** object);
    int translateLambda(SExp* sexp, Object** object);
    int translateLet(SExp* sexp, Object** object);
    int translateLetAsterisk(SExp* sexp, Object** object);
    int translateSet(SExp* sexp, Object** object);
    int translateApplication(SExp* sexp, Object** object);
    int translateQuote(SExp* sexp, Object** object);
    int translateEval(SExp* sexp, Object** object);
};

}; // namespace monash

#endif // __TRANSLATOR_H__
