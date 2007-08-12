/*!
    \file  Translator.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __TRANSLATOR_H__
#define __TRANSLATOR_H__

#include "SExp.h"
#include "Object.h"
#include "Procedure.h"
#include "Number.h"
#include "Charcter.h"
#include "SString.h"
#include "SpecialIf.h"
#include "Assignment.h"
#include "Begin.h"
#include "Definition.h"
#include "Application.h"
#include "Cond.h"
#include "Unquote.h"
#include "UnquoteSplicing.h"
#include "QuasiQuote.h"
#include "Let.h"
#include "NamedLet.h"
#include "LetAsterisk.h"
#include "Macro.h"
#include "SExp.h"
#include "Pair.h"

namespace monash {

#ifdef USE_BOEHM_GC
class Translator : public gc_cleanup
//class SExp : public gc
#else
class Translator
#endif
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

//    Object* translateBegin2(Pair* data);
    Object* translate2(Object* data);

    int translateAsQuote(SExp* sexp, Object** object);
    int translateAsQuasiQuote(SExp* sexp, Object** object);
    int translate(SExp** sexp, Object** object);
    int translateDefineSyntax(SExp* sexp);
    SExp* expandMacroIfMatch(const ::util::String& name, SExp** sexp);
private:
    int translateAsQuotePrimitive(SExp* sexp, Object** object);
    int translateAsVectorQuote(SExp* sexp, Object** object);
    int translateAsListQuote(SExp* sexp, Object** object);
    int translateAsVectorQuasiQuote(SExp* sexp, Object** object);
    int translateAsListQuasiQuote(SExp* sexp, Object** object);

    int translatePrimitive(SExp* sexp, Object** object);
    int translateDefinition(SExp* sexp, Object** object);
    int translateDefinitionMacro(SExp* sexp, Object** object);

    int translateIf(SExp* sexp, Object** object);
    int translateAnd(SExp* sexp, Object** object);
    int translateOr(SExp* sexp, Object** object);
    int translateCond(SExp* sexp, Object** object);
    int translateBegin(SExp* sexp, Object** object);
    int translateLambda(SExp* sexp, Object** object);
    int translateLet(SExp* sexp, Object** object);
    int translateNamedLet(SExp* sexp, Object** object);
    int translateLetAsterisk(SExp* sexp, Object** object);
    int translateSet(SExp* sexp, Object** object);
    int translateApplication(SExp* sexp, Object** object);
    int translateQuote(SExp* sexp, Object** object);
    int translateQuasiQuote(SExp* sexp, Object** object);
    int translateQuasiQuoteList(SExp* sexp, Object** object);
    int translateQuasiQuoteData(SExp* sexp, Object** object);
    int translateUnquote(SExp* sexp, Object** object);

    // ugly
    bool inLambda_;
};

}; // namespace monash

#endif // __TRANSLATOR_H__
