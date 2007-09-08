/*!
    \file  Translator.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __TRANSLATOR2_H__
#define __TRANSLATOR2_H__

#include "Object.h"
#include "Procedure.h"
#include "Number.h"
#include "Charcter.h"
#include "SString.h"
#include "SIf.h"
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
#include "Cons.h"

namespace monash {

class Cons;

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
    Object* translate(Object* sexp);
    void translateBody(Object* body, Objects* body, const ::util::String& syntax);
private:
    Object* translateList(Cons* cons);
    Object* translateIf(Cons* cons);
    Object* translateLambda(Cons* cons);
    Object* translateAssignment(Cons* cons);
    Object* translateBegin(Cons* cons);
    Object* translateAnd(Cons* cons);
    Object* translateOr(Cons* cons);
    Object* translateLet(Cons* cons);
    Object* translateLetAsterisk(Cons* cons);
    Object* translateNamedLet(Cons* cons);
    Object* translateProcedureCall(Cons* cons);
    Object* translateCond(Cons* cons);
    Object* translateDefinition(Cons* cons);
    Object* translateQuasiQuote(Cons* cons);

    Object* translateQuasiQuoteIter(Object* o, bool isUnquote = false);
    void translateBindingSpec(Object* bindingSpecs, Variables* bindings, Cons* specs);

    // ugly
    bool inLambda_;
};

}; // namespace monash

#endif // __TRANSLATOR_H__
