/*!
    \file   Translator.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "Translator.h"

using namespace util;
using namespace monash;
#define SYNTAX_ERROR(...) printf(__VA_ARGS__);fflush(stdout);

Translator::Translator() : inLambda_(false)
{
}

Translator::~Translator()
{
}

// <expression> => <variable>
//               | <literal>
//               | <procedure call>
//               | <lambda expression>
//               | <conditional>
//               | <assignment>
//               | <derived expression>
//               | <macro use>
//               | <macro block>
Object* Translator::translate(Object* sexp)
{
    // <variable>
    if (sexp->isIdentifier())
    {
        Identifier* r = (Identifier*)sexp;
        Object* ret = new Variable(r->text(), r->lineno());
        SCM_ASSERT(ret);
        return ret;
    }
    // <literal> => <quotation> | <self-evaluating>
    //      <self-evaluating> -> <boolean> | <number> |<character> | <string> | <regexp> | <regmatch>
    else if (sexp->isTrue()        ||
             sexp->isFalse()       ||
             sexp->isNumber()      ||
             sexp->isCharcter()    ||
             sexp->isSString()     ||
             sexp->isEnvironment() ||
             sexp->isUndef()       ||
             sexp->isSRegMatch()   ||
             sexp->isSRegexp())
    {
        return sexp;
    }
    else if (sexp->isCons())
    {
        // この時点で proper list であることを保証しよう
        return translateList((Cons*)sexp);
    }
    else
    {
        RAISE_ERROR(sexp->lineno(), "pair required, but got %s", sexp->typeString().data());
        return NULL;
    }
}

Object* Translator::translateList(Cons* cons)
{
    Identifier* car;
    if (cons->getCar()->isIdentifier())
    {
        car = (Identifier*)cons->getCar();

        const String& label = car->text();
        // <conditional> => <if> <test> <consequent> <alternate>
        if (label == "if")
        {
            return translateIf(cons);
        }
        // <lambda expression> =>  (lambda <formals> <body>)
        else if (label == "lambda")
        {
            return translateLambda(cons);
        }
        // <assignment> => (set! <variable> <expression>)
        else if (label == "set!")
        {
            return translateAssignment(cons);
        }
        // <literal> => <quotation> | <self-evaluating>
        //     <quotation> => '<datum> | (quote datum)
        else if (label == "quote")
        {
            return cons->getCadr();
        }
        // <macro block> => (let-syntax (syntax spec*) body)
        //                | (letrec-syntax (syntax spec*) body)
        //     <syntax spec> => (keyword transformer spec)
        else if (label == "let-syntax")
        {
            RAISE_ERROR(cons->lineno(), "syntax-error: let-syntax is not implemented");
        }
        else if (label == "letrec-syntax")
        {
            RAISE_ERROR(cons->lineno(), "syntax-error: letrec-syntax is not implemented");
        }
        // derived expression start
        else if (label == "cond")
        {
            return translateCond(cons);
        }
        else if (label == "and")
        {
            return translateAnd(cons);
        }
        else if (label == "or")
        {
            return translateOr(cons);
        }
        else if (label == "let")
        {
            return translateLet(cons);
        }
        else if (label == "let*")
        {
            return translateLetAsterisk(cons);
        }
// defined as macro
//         else if (label == "letrec")
//         {

//         }
        else if (label == "begin")
        {
            return translateBegin(cons);
        }
// defined as macro
//         else if (label == "do")
//         {

//         }
// defined as macro
//         else if (label == "delay")
//         {

//         }
        // derived expression end
        else if (label == "quasiquote")
        {
            return translateQuasiQuote(cons);
        }
        // <definition> => (define <variable> <expression>)
        else if (label == "define")
        {
            return translateDefinition(cons);
        }
        else if (label == "define-syntax")
        {
            return SCM_NIL;
        }
        // <procedure call>
        else
        {
            return translateProcedureCall(cons);
        }
    }
    // <procedure call>
    else
    {
        return translateProcedureCall(cons);
    }
    RAISE_ERROR(cons->lineno(), "syntax-error: not reached");
    return NULL;
}

// <conditional> => <if> <test> <consequent> <alternate>
Object* Translator::translateIf(Cons* cons)
{
    int listLength = cons->getListLength();
    if (listLength > 4 || listLength < 3)
    {
        RAISE_ERROR(cons->lineno(), "syntax-error: malformed if");
    }
    Object* test       = translate(cons->getCadr());
    Object* consequent = translate(cons->getCaddr());
    Object* alternate  = NULL;
    if (listLength == 4)
    {
        alternate = translate(cons->getCadddr());
    }
    Object* ret = new SIf(test, consequent, alternate, cons->lineno());
    SCM_ASSERT(ret);
    return ret;
}

// <lambda expression> =>  (lambda <formals> <body>)
//     <formals> => (<variable>*) | <variable>
//                | (variable+ . variable)
Object* Translator::translateLambda(Cons* cons)
{
    int listLength = cons->getListLength();
    if (listLength < 3)
    {
        RAISE_ERROR(cons->lineno(), "syntax-error: malformed lambda: length");
    }
    Variables* variables = new Variables;
    bool extendVariable = false;
    SCM_ASSERT(variables);

    Object* formals = cons->getCadr();
    // (<variable>*) | (variable+ . variable)
    if (formals->isCons())
    {
        Cons* f = (Cons*)formals;
        for (;;)
        {
            Object* car = f->getCar();
            Object* cdr = f->getCdr();
            if (!car->isIdentifier())
            {
                RAISE_ERROR(cons->lineno(), "syntax-error: malformed lambda:symbol required");
            }
            Variable* variable = new Variable(((Identifier*)car)->text(), car->lineno());
            SCM_ASSERT(variable);
            variables->add(variable);
            if (cdr->isNil())
            {
                break;
            }
            else if (cdr->isCons())
            {
                f = (Cons*)cdr;
            }
            else if (cdr->isIdentifier())
            {
                Variable* v = new Variable(".", car->lineno());
                SCM_ASSERT(v);
                variables->add(v);
                v = new Variable(((Identifier*)cdr)->text(), car->lineno());
                SCM_ASSERT(v);
                variables->add(v);
                break;
            }
            else
            {
                RAISE_ERROR(cons->lineno(), "syntax-error: malformed lambda: not proper list");
            }
        }
    }
    // '()
    else if (formals->isNil())
    {
    }
    // <variable>
    else if (formals->isIdentifier())
    {
        extendVariable = true;
        Variable* variable = new Variable(((Identifier*)formals)->text(), formals->lineno());
        SCM_ASSERT(variable);
        variables->add(variable);
    }
    else
    {
        RAISE_ERROR(cons->lineno(), "syntax-error: malformed lambda: unknown");
    }

    Objects* body = new Objects;
    SCM_ASSERT(body);
    translateBody(cons->getCddr(), body, "lambda");
    Object* ret = new Lambda(body, variables, extendVariable, false, cons->lineno());
    SCM_ASSERT(ret);
    return ret;
}

// <assignment> => (set! <variable> <expression>)
Object* Translator::translateAssignment(Cons* cons)
{
    int listLength = cons->getListLength();
    if (listLength != 3)
    {
        RAISE_ERROR(cons->lineno(), "syntax-error: malformed set!");
    }
    Object* v = cons->getCadr();
    if (!v->isIdentifier())
    {
        RAISE_ERROR(cons->lineno(), "syntax-error: malformed set!");
    }
    Variable* variable = new Variable(((Identifier*)v)->text(), cons->lineno());
    SCM_ASSERT(variable);
    Object* expression = translate(cons->getCaddr());
    Object* ret = new Set(variable, expression, cons->lineno());
    SCM_ASSERT(ret);
    return ret;
}

// derived expression <begin>
Object* Translator::translateBegin(Cons* cons)
{
    Objects* expressions = new Objects;
    SCM_ASSERT(expressions);
    translateBody(cons->getCdr(), expressions, "begin");
    Object* ret = new Begin(expressions, cons->lineno());
    SCM_ASSERT(ret);
    return ret;
}

// derived expression <and>
Object* Translator::translateAnd(Cons* cons)
{
    Object* ret = new And(cons, cons->lineno());
    SCM_ASSERT(ret);
    return ret;
}

// derived expression <or>
Object* Translator::translateOr(Cons* cons)
{
    Object* ret = new Or(cons, cons->lineno());
    SCM_ASSERT(ret);
    return ret;
}

// derived expression <let>
Object* Translator::translateLet(Cons* cons)
{
    int listLength = cons->getListLength();
    if (listLength < 3)
    {
        RAISE_ERROR(cons->lineno(), "syntax-error: malformed let");
    }

    Object* arg1 = cons->getCadr();
    if (arg1->isIdentifier())
    {
        return translateNamedLet(cons);
    }

    if (!arg1->isNil() && !arg1->isCons())
    {
        RAISE_ERROR(cons->lineno(), "syntax-error: malformed let: argument(s) not found");
    }
    Variables* bindings = new Variables;
    Cons* specs      = new Cons;
    SCM_ASSERT(bindings);
    SCM_ASSERT(specs);
    translateBindingSpec(arg1, bindings, specs);

    Objects* tailBody = new Objects;
    SCM_ASSERT(tailBody);
    translateBody(cons->getCddr(), tailBody, "let");
    Object* ret = new Let(tailBody, bindings, specs, cons->lineno());
    SCM_ASSERT(ret);
    return ret;
}

// derived expression <let*>
Object* Translator::translateLetAsterisk(Cons* cons)
{
    int listLength = cons->getListLength();
    if (listLength < 3)
    {
        RAISE_ERROR(cons->lineno(), "syntax-error: malformed let*");
    }

    Object* arg1 = cons->getCadr();
    if (arg1->isIdentifier())
    {
        return translateNamedLet(cons);
    }

    if (!arg1->isNil() && !arg1->isCons())
    {
        RAISE_ERROR(cons->lineno(), "syntax-error: malformed let*: argument(s) not found");
    }
    Variables* bindings = new Variables;
    Cons* specs = new Cons;
    SCM_ASSERT(bindings);
    SCM_ASSERT(specs);
    translateBindingSpec(arg1, bindings, specs);

    Objects* tailBody = new Objects;
    SCM_ASSERT(tailBody);
    translateBody(cons->getCddr(), tailBody, "let*");
    Object* ret = new LetAsterisk(tailBody, bindings, specs, cons->lineno());
    SCM_ASSERT(ret);
    return ret;
}

// derived expression <cond>
//      (cond <cond clause>+)
//    | (cond <cond clause>* (else <sequence>))
//    | (case <expression>
//        <case clause>+)
//    | (case <expression>
//        <case clause>*
//        (else <sequence>))
Object* Translator::translateCond(Cons* cons)
{
    Clauses* clauses = new Clauses;
    SCM_ASSERT(clauses);
    Objects* elseActions = NULL;
    if (!cons->getCdr()->isCons())
    {
        RAISE_ERROR(cons->lineno(), "syntax-error: malformed cond");
    }
    Cons* c = (Cons*)cons->getCdr();
    for (;;)
    {
        Object* car = c->getCar();
        Object* cdr = c->getCdr();
        if (!car->isCons() || ((Cons*)car)->getListLength() < 2)
        {
            RAISE_ERROR(cons->lineno(), "syntax-error: malformed cond");
        }
        Cons* kar = (Cons*)car;

        // (else <sequence>))
        if (cdr->isNil()
            && kar->getCar()->isIdentifier()
            && ((Identifier*)(kar->getCar()))->text() == "else")
        {
            elseActions = new Objects;
            SCM_ASSERT(elseActions);
            translateBody(kar->getCdr(), elseActions, "cond else");
        }
        else
        {
            // (cond (1 => foo))
            if (kar->getListLength() == 3
                && kar->getCadr()->isIdentifier()
                && ((Identifier*)(kar->getCadr()))->text() == "=>")
            {
                Object* cond = translate(kar->getCar());
                Object* action = translate(kar->getCaddr());
                Cons* arguments = new Cons;
                SCM_ASSERT(arguments);
                arguments->append(cond);
                Object* application = new Application(action, arguments, action->lineno());
                SCM_ASSERT(application);
                Objects* actions = new Objects;
                SCM_ASSERT(actions);
                actions->add(application);
                Clause* c = new Clause(cond, actions);
                SCM_ASSERT(c);
                clauses->add(c);
            }
            // (pred action)
            else
            {
                Object* cond = translate(kar->getCar());
                Objects* actions = new Objects;
                SCM_ASSERT(actions);
                translateBody(kar->getCdr(), actions, "cond");
                Clause* c = new Clause(cond, actions);
                SCM_ASSERT(c);
                clauses->add(c);
            }
        }

        if (cdr->isNil())
        {
            break;
        }
        else if (cdr->isCons())
        {
            c = (Cons*)cdr;
        }
        else
        {
            RAISE_ERROR(cons->lineno(), "syntax-error: malformed named");
        }
    }
    Object* ret = new Cond(clauses, elseActions, cons->lineno());
    SCM_ASSERT(ret);
    return ret;
}

// derived expression <named let>
// (let <variable> (<binding spec>*) <tail body>)
Object* Translator::translateNamedLet(Cons* cons)
{
    // (<binding spec>*)
    Object* bindingSpecs = cons->getCaddr();
    Variables* bindings = new Variables;
    Cons* specs      = new Cons;
    SCM_ASSERT(bindings);
    SCM_ASSERT(specs);
    translateBindingSpec(bindingSpecs, bindings, specs);

    // <tail body>
    Objects* tailBody = new Objects;
    SCM_ASSERT(tailBody);
    translateBody(cons->getCdddr(), tailBody, "named let");

    // <variable>
    Object* arg1 = cons->getCadr();
    if (!arg1->isIdentifier())
    {
        RAISE_ERROR(cons->lineno(), "syntax-error: malformed named let");
    }
    Object* ret = new NamedLet(tailBody, bindings, specs, ((Identifier*)arg1)->text(), cons->lineno());
    SCM_ASSERT(ret);
    return ret;
}

// <procedure call> => (<operator> <operand>*)
//     <operator> => <expresson>
//     <operand>  => <expresson>
Object* Translator::translateProcedureCall(Cons* cons)
{
    Object* operatorr  = translate(cons->getCar());
//     Objects* operands = new Objects;
//     SCM_ASSERT(operands);
//     if (cons->getListLength() >= 2)
//     {
//         translateBody(cons->getCdr(), operands, "procedure call");
//     }
    Cons* operands = NULL;
    Object* cdr = cons->getCdr();
    if (!cdr->isNil() && cdr->isCons())
    {
        operands = (Cons*)cdr;
    }

    Object* ret = new Application(operatorr, operands, cons->lineno());
    SCM_ASSERT(ret);
    return ret;
}

// for <let>, <named let> and <begin>
void Translator::translateBody(Object* bodyCons, Objects* body, const ::util::String& syntax)
{
    if (!bodyCons->isCons())
    {
        RAISE_ERROR(bodyCons->lineno(), "syntax-error: malformed %s 1", syntax.data());
    }
    Cons* c = (Cons*)bodyCons;
    for (;;)
    {
        Object* car = c->getCar();
        Object* cdr = c->getCdr();
        body->add(translate(car));
        if (cdr->isNil())
        {
            break;
        }
        else if (cdr->isCons())
        {
            c = (Cons*)cdr;
        }
        else
        {
            RAISE_ERROR(bodyCons->lineno(), "syntax-error: malformed %s 2", syntax.data());
        }
    }
}

// for <let> and <named let>
void Translator::translateBindingSpec(Object* bindingSpecs, Variables* bindings, Cons* specs)
{
    Cons* currentSpecs = specs;
    if (bindingSpecs->isCons())
    {
        Cons* c = (Cons*)bindingSpecs;
        for (;;)
        {
            Object* car = c->getCar();
            if (!car->isCons())
            {
                RAISE_ERROR(bindingSpecs->lineno(), "syntax-error: malformed let or named let: malformed <binding spec>1");
            }
            Cons* bindingSpec = (Cons*)car;

            if (bindingSpec->getListLength() != 2 || !bindingSpec->getCar()->isIdentifier())
            {
                RAISE_ERROR(bindingSpecs->lineno(), "syntax-error: malformed let or named let: malformed <binding spec>2");
            }
            Variable* binding = new Variable(((Identifier*)bindingSpec->getCar())->text(), bindingSpecs->lineno());
            SCM_ASSERT(binding);
            bindings->add(binding);
//            specs->add(translate(bindingSpec->getCadr()));
            if (currentSpecs->getCar() == NULL)
            {
                currentSpecs->setCar(bindingSpec->getCadr());
            }
            else
            {
                Cons* tmp = new Cons(bindingSpecs->lineno());
                tmp->setCar(bindingSpec->getCadr());
                currentSpecs->setCdr(tmp);
                currentSpecs = tmp;
            }

            Object* cdr = c->getCdr();
            if (cdr->isNil())
            {
                break;
            }
            else if (cdr->isCons())
            {
                c = (Cons*)cdr;
            }
            else
            {
                RAISE_ERROR(bindingSpecs->lineno(), "syntax-error: malformed let or named let");
            }
        }
    }
    else if (bindingSpecs->isNil())
    {

    }
    else
    {
        RAISE_ERROR(bindingSpecs->lineno(), "syntax-error: malformed let or named let: malformed <binding spec>");
    }
}

Object* Translator::translateQuasiQuote(Cons* cons)
{
    return new QuasiQuote(translateQuasiQuoteIter(cons->getCadr()), cons->lineno());
}

Object* Translator::translateQuasiQuoteIter(Object* o, bool isUnquote)
{
    if (!o->isCons())
    {
        if (isUnquote && o->isIdentifier())
        {
            return o;//new Variable(((Identifier*)o)->text(), o->lineno());
        }
        return o;
    }
    Cons* cons = (Cons*)o;
    if (cons->getListLength() == 2 && cons->getCar()->isIdentifier())
    {
        const String& name = ((Identifier*)cons->getCar())->text();
        if (name == "unquote")
        {
            return new Unquote(translateQuasiQuoteIter(cons->getCadr(), true), cons->lineno());
        }
        else if (name == "unquote-splicing")
        {
            return new UnquoteSplicing(translateQuasiQuoteIter(cons->getCadr(), true), cons->lineno());
        }
    }

    cons->setCar(translateQuasiQuoteIter(cons->getCar(), isUnquote));
    cons->setCdr(translateQuasiQuoteIter(cons->getCdr(), isUnquote));
    return cons;
}

Object* Translator::translateDefinition(Cons* cons)
{
//    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    Object* kadr = cons->getCadr();
    if (kadr->isCons())
    {
        Cons* c = (Cons*)kadr;
        if (!c->getCar()->isIdentifier())
        {
            RAISE_ERROR(cons->lineno(), "syntax-error: malformed define");
        }
        Variable* variable = new Variable(((Identifier*)c->getCar())->text(), cons->lineno());
        SCM_ASSERT(variable);
        Variables* variables = new Variables;
        SCM_ASSERT(variables);

        Object* kdr = c->getCdr();
        if (kdr->isCons())
        {
            c = (Cons*)kdr;
            for (;;)
            {
                Object* car = c->getCar();
                Object* cdr = c->getCdr();
                if (!car->isIdentifier())
                {
                    RAISE_ERROR(cons->lineno(), "syntax-error: malformed define");
                }
                Variable* v = new Variable(((Identifier*)car)->text(), cons->lineno());
                SCM_ASSERT(v);
                variables->add(v);
                if (cdr->isNil())
                {
                    break;
                }
                else if (cdr->isCons())
                {
                    c = (Cons*)cdr;
                }
                else if (cdr->isIdentifier())
                {
                    Variable* v = new Variable(".", cons->lineno());
                    SCM_ASSERT(v);
                    variables->add(v);
                    v = new Variable(((Identifier*)cdr)->text(), car->lineno());
                    SCM_ASSERT(v);
                    variables->add(v);
                    break;
                }
                else
                {
                    RAISE_ERROR(cons->lineno(), "syntax-error: malformed define");
                }
            }
        }
        else if (kdr->isNil())
        {
        }
        else
        {
            RAISE_ERROR(cons->lineno(), "syntax-error: malformed define");
        }

        Objects* body = new Objects;
        SCM_ASSERT(body);
        translateBody(cons->getCddr(), body, "define");
        Object* lambda = new Lambda(body, variables, false, false, cons->lineno());
        SCM_ASSERT(lambda);
        Object* ret = new Definition(variable, lambda, cons->lineno());
        SCM_ASSERT(ret);
        return ret;
    }
    else
    {
        if (cons->getListLength() != 3)
        {
            RAISE_ERROR(cons->lineno(), "syntax-error: malformed define");
        }
        if (!cons->getCadr()->isIdentifier())
        {
            RAISE_ERROR(cons->lineno(), "syntax-error: malformed define");
        }
        Variable* variable = new Variable(((Identifier*)(cons->getCadr()))->text(), cons->lineno());
        SCM_ASSERT(variable);
        Object* expresson = translate(cons->getCaddr());
        Object* ret = new Definition(variable, expresson, cons->lineno());
        SCM_ASSERT(ret);
        return ret;
    }
}



// int Translator::translateAsQuasiQuote(SExp* sexp, Object** object)
// {
//     if (sexp->type != SExp::SEXPS)
//     {
//         // sama as quote
//         return translateAsQuotePrimitive(sexp, object);
//     }

//     if (L() > 1 && N(0)->text == "VECTOR")
//     {
//         return translateAsVectorQuasiQuote(sexp, object);
//     }
//     else
//     {
//         if (L() > 0 && N(0)->text == "lambda") inLambda_ = true;
//         int ret = translateAsListQuasiQuote(sexp, object);
//         if (inLambda_) inLambda_ = false;
//         return ret;
//     }
// }

// int Translator::translateAsVectorQuote(SExp* sexp, Object** object)
// {
//     SCM_ASSERT(L() > 0);
//     SCM_ASSERT(N(0)->text == "VECTOR");
//     Vector* v = new Vector(L() - 1, sexp->lineno);
//     for (int i = 1; i < L(); i++)
//     {
//         Object* o;
//         if (translateAsQuote(N(i), &o) != SUCCESS) return SYNTAX_ERROR;
//         v->set(i - 1, o);
//     }
//     *object = v;
//     return SUCCESS;
// }

// int Translator::translateAsVectorQuasiQuote(SExp* sexp, Object** object)
// {
//     SCM_ASSERT(L() > 0);
//     SCM_ASSERT(N(0)->text == "VECTOR");
//     Vector* v = new Vector(L() - 1, sexp->lineno);
//     for (int i = 1; i < L(); i++)
//     {
//         Object* o;
//         if (translateQuasiQuoteData(N(i), &o) != SUCCESS) return SYNTAX_ERROR;
//         v->set(i - 1, o);
//     }
//     *object = v;
//     return SUCCESS;
// }

// int Translator::translateAsListQuote(SExp* sexp, Object** object)
// {
//     // ()
//     if (L() == 0)
//     {
//         *object = SCM_NIL;
//         return SUCCESS;
//     }

//     // (a . b)
//     if (!inLambda_ && L() == 3 && N(1)->isSymbol() && N(1)->text == ".")
//     {
//         Object* car;
//         Object* cdr;
//         if (translateAsQuote(N(0), &car) != SUCCESS) return SYNTAX_ERROR;
//         if (translateAsQuote(N(2), &cdr) != SUCCESS) return SYNTAX_ERROR;
//         *object = new Cons(car, cdr, sexp->lineno);
//         return SUCCESS;
//     }

//     // (a b c d)
//     Cons* start = new Cons(SCM_NIL, SCM_NIL, sexp->lineno);
//     Cons* p = start;
//     for (int i = 0; i < L(); i++)
//     {
//         Object* car;
//         if (translateAsQuote(N(i), &car) != SUCCESS) return SYNTAX_ERROR;
//         p->setCar(car);
//         if (i != L() -1)
//         {
//             Cons* tmp = new Cons(SCM_NIL, SCM_NIL, sexp->lineno);
//             p->setCdr(tmp);
//             p = tmp;
//         }
//     }
//     *object = start;
//     return SUCCESS;
// }

// int Translator::translateAsListQuasiQuote(SExp* sexp, Object** object)
// {
//     // ()
//     if (L() == 0)
//     {
//         *object = SCM_NIL;
//         return SUCCESS;
//     }

//     // (a . b)
//     if (!inLambda_ && L() == 3 && N(1)->isSymbol() && N(1)->text == ".")
//     {
//         Object* car;
//         Object* cdr;
//         if (translateQuasiQuoteData(N(0), &car) != SUCCESS) return SYNTAX_ERROR;
//         if (translateQuasiQuoteData(N(2), &cdr) != SUCCESS) return SYNTAX_ERROR;
//         *object = new Cons(car, cdr, sexp->lineno);
//         return SUCCESS;
//     }

//     // (a b c d)
//     Cons* start = new Cons(SCM_NIL, SCM_NIL, sexp->lineno);
//     Cons* p = start;
//     for (int i = 0; i < L(); i++)
//     {
//         Object* car;
//         if (translateQuasiQuoteData(N(i), &car) != SUCCESS) return SYNTAX_ERROR;
//         p->setCar(car);
//         if (i != L() -1)
//         {
//             Cons* tmp = new Cons(SCM_NIL, SCM_NIL, sexp->lineno);
//             p->setCdr(tmp);
//             p = tmp;
//         }
//     }
//     *object = start;
//     return SUCCESS;
// }

// int Translator::translateAsQuotePrimitive(SExp* sexp, Object** object)
// {
//     switch(sexp->type)
//     {
//     case SExp::NUMBER:
// #ifdef USE_MONA_GC
//         *object = new(false) Number(sexp->value, sexp->lineno);SCM_ASSERT(*object);
// #else
//         *object = new Number(sexp->value, sexp->lineno);SCM_ASSERT(*object);
// #endif
//         return SUCCESS;
//     case SExp::STRING:
//         *object = new SString(sexp->text, sexp->lineno);SCM_ASSERT(*object);
//         return SUCCESS;
//     case SExp::CHAR:
// #ifdef USE_MONA_GC
//         *object = new(false) Charcter(sexp->text, sexp->lineno);SCM_ASSERT(*object);
// #else
//         *object = new Charcter(sexp->text, sexp->lineno);SCM_ASSERT(*object);
// #endif
//         return SUCCESS;
//     case SExp::SYMBOL:
//         if (sexp->text == "#t")
//         {
//             *object = SCM_TRUE;
//         }
//         else if (sexp->text == "#f")
//         {
//             *object = SCM_FALSE;
//         }
//         else
//         {
//             *object = new Identifier(sexp->text, sexp->lineno);SCM_ASSERT(*object);
//         }
//         return SUCCESS;
//     case SExp::REGEXP:
//         *object = new SRegexp(sexp->text, sexp->value == 1, sexp->lineno);SCM_ASSERT(*object);
//         return SUCCESS;
//     }
//     return SYNTAX_ERROR;
// }



// int Translator::translatePrimitive(SExp* sexp, Object** object)
// {
//     switch(sexp->type)
//     {
//     case SExp::NUMBER:
// #ifdef USE_MONA_GC
//         *object = new(false) Number(sexp->value, sexp->lineno);SCM_ASSERT(*object);
// #else
//         *object = new Number(sexp->value, sexp->lineno);SCM_ASSERT(*object);
// #endif
//         return SUCCESS;
//     case SExp::STRING:
// #ifdef USE_MONA_GC
//         *object = new SString(sexp->text, sexp->lineno);SCM_ASSERT(*object);
// #else
//         *object = new SString(sexp->text, sexp->lineno);SCM_ASSERT(*object);
// #endif
//         return SUCCESS;
// //     case SExp::QUOTE:
// //         printf("quote:%s\n", sexp->text.data());
// //         *object = new Quote(SExp::fromString(sexp->text), sexp->lineno);SCM_ASSERT(*object);
// //         return SUCCESS;
//     case SExp::CHAR:
// #ifdef USE_MONA_GC
//         *object = new(false) Charcter(sexp->text, sexp->lineno);SCM_ASSERT(*object);
// #else
//         *object = new Charcter(sexp->text, sexp->lineno);SCM_ASSERT(*object);
// #endif
//         return SUCCESS;
//     case SExp::SYMBOL:
// #ifdef USE_MONA_GC
//         *object = new Variable(sexp->text, sexp->lineno);SCM_ASSERT(*object);
// #else
//         *object = new Variable(sexp->text, sexp->lineno);SCM_ASSERT(*object);
// #endif
//         return SUCCESS;
//     case SExp::REGEXP:
//         *object = new SRegexp(sexp->text, sexp->value == 1, sexp->lineno);SCM_ASSERT(*object);
//         return SUCCESS;
//     }
//     printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
//     return SYNTAX_ERROR;
// }

// int Translator::translateDefinition(SExp* sexp, Object** object)
// {
// #if 0
//     if (L() != 3) return SYNTAX_ERROR;
//     SExp* symbol = N(1);
//     if (symbol->type != SExp::SYMBOL) return SYNTAX_ERROR;
//     Variable* variable = new Variable(symbol->text, symbol->lineno);SCM_ASSERT(variable);
//     SExp* argument = N(2);
//     Object* argumentObject;
//     if (translate(&argument, &argumentObject) != SUCCESS) return SYNTAX_ERROR;
//     *object = new Definition(variable, argumentObject, sexp->lineno);SCM_ASSERT(*object);
//     return SUCCESS;
// #else
//     if (N(1)->isSExps())
//     {
//         Variable* variable = new Variable(NN(1, 0)->text, NN(1, 0)->lineno);SCM_ASSERT(variable);
//         Variables* variables = new Variables;
//         for (int i = 1; i < LL(1); i++)
//         {
//             Variable* v = new Variable(NN(1, i)->text, NN(1, i)->lineno);SCM_ASSERT(v);
//             variables->add(v);
//         }
//         Objects* body = new Objects;
//         for (int i = 2; i < L(); i++)
//         {
//             Object* b;
//             SExp* s = N(i);
//             if (translate(&s, &b) != SUCCESS) {printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);return SYNTAX_ERROR;}
//             body->add(b);
//         }
//         Object* l = new Lambda(body, variables, false, sexp->lineno);SCM_ASSERT(l);
//         *object = new Definition(variable, l, sexp->lineno);SCM_ASSERT(*object);
//         return SUCCESS;
//     }
//     else
//     {
//         if (L() != 3) {printf("%s %s:%d L()=%d %s %s\n", __func__, __FILE__, __LINE__, L(), N(0)->toSExpString().data(), N(3)->toSExpString().data());fflush(stdout);return SYNTAX_ERROR;}
//         SExp* symbol = N(1);
//         if (symbol->type != SExp::SYMBOL) {printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);return SYNTAX_ERROR;}
//         Variable* variable = new Variable(symbol->text, symbol->lineno);SCM_ASSERT(variable);
//         SExp* argument = N(2);
//         Object* argumentObject;
//         if (translate(&argument, &argumentObject) != SUCCESS) {printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);return SYNTAX_ERROR;}
//         *object = new Definition(variable, argumentObject, sexp->lineno);SCM_ASSERT(*object);
//         return SUCCESS;
//     }
// #endif
// }

// int Translator::translateDefinitionMacro(SExp* sexp, Object** object)
// {
// #if 0
//     if (L() != 3) return SYNTAX_ERROR;
//     SExp* symbol = N(1);
//     if (symbol->type != SExp::SYMBOL) return SYNTAX_ERROR;
//     Variable* variable = new Variable(symbol->text, symbol->lineno);SCM_ASSERT(variable);
//     SExp* argument = N(2);
//     Object* argumentObject;
//     if (translate(&argument, &argumentObject) != SUCCESS) return SYNTAX_ERROR;
//     *object = new Definition(variable, argumentObject, sexp->lineno);SCM_ASSERT(*object);
//     return SUCCESS;
// #else
//     if (N(1)->isSExps())
//     {
//         Variable* variable = new Variable(NN(1, 0)->text, NN(1, 0)->lineno);SCM_ASSERT(variable);
//         Variables* variables = new Variables;
//         for (int i = 1; i < LL(1); i++)
//         {
//             Variable* v = new Variable(NN(1, i)->text, NN(1, i)->lineno);SCM_ASSERT(v);
//             variables->add(v);
//         }
//         Objects* body = new Objects;
//         for (int i = 2; i < L(); i++)
//         {
//             Object* b;
//             SExp* s = N(i);
//             if (translate(&s, &b) != SUCCESS) {printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);return SYNTAX_ERROR;}
//             body->add(b);
//         }
//         Object* l = new Lambda(body, variables, false, true, sexp->lineno);SCM_ASSERT(l);
//         *object = new Definition(variable, l, sexp->lineno);SCM_ASSERT(*object);
//         return SUCCESS;
//     }
//     else if (!N(1)->isSExps() && N(2)->isSExps())
//     {
//         Variable* variable = new Variable(N(1)->text, N(1)->lineno);SCM_ASSERT(variable);
//         Variables* variables = new Variables;
//         for (int i = 0; i < LL(2); i++)
//         {
//             Variable* v = new Variable(NN(2, i)->text, NN(2, i)->lineno);SCM_ASSERT(v);
//             variables->add(v);
//         }
//         Objects* body = new Objects;
//         for (int i = 3; i < L(); i++)
//         {
//             Object* b;
//             SExp* s = N(i);
//             if (translate(&s, &b) != SUCCESS) {printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);return SYNTAX_ERROR;}
//             body->add(b);
//         }
//         Object* l = new Lambda(body, variables, false, true, sexp->lineno);SCM_ASSERT(l);
//         *object = new Definition(variable, l, sexp->lineno);SCM_ASSERT(*object);
//         return SUCCESS;
//     }
//     else
//     {
//         if (L() != 3) {printf("%s %s:%d L()=%d %s %s\n", __func__, __FILE__, __LINE__, L(), N(0)->toSExpString().data(), N(3)->toSExpString().data());fflush(stdout);return SYNTAX_ERROR;}
//         SExp* symbol = N(1);
//         if (symbol->type != SExp::SYMBOL) {printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);return SYNTAX_ERROR;}
//         Variable* variable = new Variable(symbol->text, symbol->lineno);SCM_ASSERT(variable);
//         SExp* argument = N(2);
//         Object* argumentObject;
//         if (translate(&argument, &argumentObject) != SUCCESS) {printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);return SYNTAX_ERROR;}
//         *object = new Definition(variable, argumentObject, sexp->lineno);SCM_ASSERT(*object);
//         return SUCCESS;
//     }
// #endif
// }


// int Translator::translateIf(SExp* sexp, Object** object)
// {
//     if (L() > 4 || L() < 3) return SYNTAX_ERROR;
//     Object* predicate = NULL;
//     Object* consequent = NULL;
//     Object* alternative = NULL;
//     SExp* n1 = N(1);
//     int ret = translate(&n1, &predicate);
//     if (ret != SUCCESS) return ret;
//     SExp* n2 = N(2);
//     ret = translate(&n2, &consequent);
//     if (ret != SUCCESS) return ret;
//     if (L() == 4)
//     {
//         SExp* n3 = N(3);
//         ret = translate(&n3, &alternative);
//         if (ret != SUCCESS) return ret;
//     }
//     *object = new SIf(predicate, consequent, alternative, sexp->lineno);SCM_ASSERT(*object);
//     return SUCCESS;
// }

// int Translator::translateCond(SExp* sexp, Object** object)
// {
//     Clauses* clauses = new Clauses;SCM_ASSERT(clauses);
//     Objects* elseActions = NULL;
//     for (int i = 1; i < L(); i++)
//     {
//         SExp* n = sexp->sexps[i];
//         if (n->sexps.size() < 2) return SYNTAX_ERROR;
//         if (i == L() - 1 && n->sexps[0]->type == SExp::SYMBOL && n->sexps[0]->text == "else")
//         {
//             elseActions = new Objects;SCM_ASSERT(elseActions);
//             for (int j = 1; j < n->sexps.size(); j++)
//             {
//                 Object * action;
//                 SExp* nj = n->sexps[j];
//                 int ret = translate(&nj, &action);
//                 if (ret != SUCCESS) return ret;
//                 elseActions->add(action);
//             }
//         }
//         else
//         {
//             // (cond (1 => hoge))
//             if (n->sexps.size() == 3 && n->sexps[1]->type == SExp::SYMBOL && n->sexps[1]->text == "=>")
//             {
//                 Object* cond;
//                 SExp* n0 = n->sexps[0];
//                 int ret = translate(&n0, &cond);
//                 if (ret != SUCCESS) return ret;
//                 Object* action;
//                 SExp* n2 = n->sexps[2];
//                 ret = translate(&n2, &action);
//                 if (ret != SUCCESS) return ret;
//                 Objects* arguments = new Objects;SCM_ASSERT(arguments);
//                 arguments->add(cond);
//                 Object* application = new Application(action, arguments, action->lineno());SCM_ASSERT(application);
//                 Objects* actions = new Objects;SCM_ASSERT(actions);
//                 actions->add(application);
//                 Clause* c = new Clause(cond, actions);
//                 SCM_ASSERT(c);
//                 clauses->add(c);
//             }
//             else
//             {
//                 Object* cond;
//                 SExp* n0 = n->sexps[0];
//                 int ret = translate(&n0, &cond);
//                 if (ret != SUCCESS) return ret;
//                 Objects* actions = new Objects;SCM_ASSERT(actions);
//                 for (int j = 1; j < n->sexps.size(); j++)
//                 {
//                     Object * action;
//                     SExp* nj = n->sexps[j];
//                     ret = translate(&nj, &action);
//                     if (ret != SUCCESS) return ret;
//                     actions->add(action);
//                 }
//                 Clause* c = new Clause(cond, actions);
//                 SCM_ASSERT(c);
//                 clauses->add(c);
//             }
//         }
//     }
//     *object = new Cond(clauses, elseActions, sexp->lineno);SCM_ASSERT(*object);
//     return SUCCESS;
// }

// int Translator::translateBegin(SExp* sexp, Object** object)
// {
//     if (L() <= 1) return SYNTAX_ERROR;
//     Objects* objects = new Objects;SCM_ASSERT(objects);
//     for (int i = 1; i < L(); i++)
//     {
//         Object* object;
//         SExp* ni = N(i);
//         int ret = translate(&ni, &object);
//         if (ret != SUCCESS) return ret;
//         objects->add(object);
//     }
//     *object = new Begin(objects, sexp->lineno);SCM_ASSERT(*object);
//     return SUCCESS;
// }

// int Translator::translateQuote(SExp* sexp, Object** object)
// {
//     if (L() < 1)
//     {
//         return SYNTAX_ERROR;
//     }
//     translateAsQuote(sexp->sexps[1], object);
//     return SUCCESS;
// }

// int Translator::translateQuasiQuote(SExp* sexp, Object** object)
// {
//     if (L() < 1)
//     {
//         return SYNTAX_ERROR;
//     }
//     Object* q;
//     if (translateQuasiQuoteData(sexp->sexps[1], &q) != SUCCESS)
//     {
//         return SYNTAX_ERROR;
//     }
//     *object = new QuasiQuote(q, sexp->lineno);
//     return SUCCESS;
// }

// int Translator::translateQuasiQuoteData(SExp* sexp, Object** object)
// {
//     if (sexp->type == SExp::SEXPS && L() > 0 && N(0)->text == "unquote")
//     {
//         Object* unquote;
//         SExp* n1 = N(1);
//         if (translate(&n1, &unquote) != SUCCESS) return SYNTAX_ERROR;
//         *object = new Unquote(unquote, sexp->lineno);
//         return SUCCESS;
//     }
//     else if (sexp->type == SExp::SEXPS && L() > 0 && N(0)->text == "unquote-splicing")
//     {
//         Object* unquote;
//         SExp* n1 = N(1);
//         if (translate(&n1, &unquote) != SUCCESS) return SYNTAX_ERROR;
//         *object = new UnquoteSplicing(unquote, sexp->lineno);
//         return SUCCESS;
//     }
//     else
//     {
//         return translateAsQuasiQuote(sexp, object);
//     }
// }


// int Translator::translateQuasiQuoteList(SExp* sexp, Object** object)
// {
//     // ()
//     if (L() == 0)
//     {
//         *object = SCM_NIL;
//         return SUCCESS;
//     }

//     // (a . b)
//     if (!inLambda_ && L() == 3 && N(1)->isSymbol() && N(1)->text == ".")
//     {
//         Object* car;
//         Object* cdr;
//         if (translateQuasiQuoteData(N(0), &car) != SUCCESS) return SYNTAX_ERROR;
//         if (translateQuasiQuoteData(N(2), &cdr) != SUCCESS) return SYNTAX_ERROR;
//         *object = new Cons(car, cdr, sexp->lineno);
//         return SUCCESS;
//     }

//     // (a b c d)
//     Cons* start = new Cons(SCM_NIL, SCM_NIL, sexp->lineno);
//     Cons* p = start;
//     for (int i = 0; i < L(); i++)
//     {
//         Object* car;
//         if (translateQuasiQuoteData(N(i), &car) != SUCCESS) return SYNTAX_ERROR;
//         p->setCar(car);
//         if (i != L() -1)
//         {
//             Cons* tmp = new Cons(SCM_NIL, SCM_NIL, sexp->lineno);
//             p->setCdr(tmp);
//             p = tmp;
//         }
//     }
//     *object = start;
//     return SUCCESS;
// }


// int Translator::translateUnquote(SExp* sexp, Object** object)
// {
//     if (L() < 1)
//     {
//         return SYNTAX_ERROR;
//     }
//     translateAsQuote(sexp->sexps[1], object);
//     return SUCCESS;
// }


// int Translator::translateLambda(SExp* sexp, Object** object)
// {
//     if (L() <= 2)
//     {
//         printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
//         return SYNTAX_ERROR;
//     }
//     if (N(1)->type != SExp::SEXPS && N(1)->type != SExp::SYMBOL)
//     {
//         printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
//         return SYNTAX_ERROR;
//     }
//     bool extendVariable = false;
//     Variables* variables = new Variables;SCM_ASSERT(variables);
//     if (N(1)->type == SExp::SEXPS)
//     {

//         for (int i = 0; i < N(1)->sexps.size(); i++)
//         {
//             SExp* param = NN(1, i);
//             if (param->type != SExp::SYMBOL)
//             {
//                 printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
//                 return SYNTAX_ERROR;
//             }
//             Variable* v = new Variable(param->text, param->lineno);
//             SCM_ASSERT(v);
//             variables->add(v);
//         }
//     }
//     else if (N(1)->type == SExp::SYMBOL)
//     {
//         Variable* v = new Variable(N(1)->text, N(1)->lineno);
//         extendVariable = true;
//         variables->add(v);
//     }
//     Objects* body = new Objects;SCM_ASSERT(body);
//     for (int i = 2; i < L(); i++)
//     {
//         Object* o;
//         SExp* ni = N(i);
//         int ret = translate(&ni, &o);
//         if (ret != SUCCESS)
//         {
//             printf("%s %s:%d:i=%d\n", __func__, __FILE__, __LINE__, i);fflush(stdout);// debug
//             return ret;
//         }
//         body->add(o);
//     }
//     *object = new Lambda(body, variables, extendVariable, sexp->lineno);SCM_ASSERT(*object);
//     return SUCCESS;
// }

// int Translator::translateNamedLet(SExp* sexp, Object** object)
// {
//     if (N(2)->type != SExp::SEXPS)
//     {
//         return SYNTAX_ERROR;
//     }
//     Variables* variables = new Variables;SCM_ASSERT(variables);
//     Objects* values = new Objects;SCM_ASSERT(values);
//     SExps* parameterSExps = &N(2)->sexps;
//     for (int i = 0; i < parameterSExps->size(); i++)
//     {
//         SExp* parameter = parameterSExps->get(i);
//         if (parameter->type != SExp::SEXPS || parameter->sexps.size() != 2) return SYNTAX_ERROR;
//         if (parameter->sexps[0]->type != SExp::SYMBOL) return SYNTAX_ERROR;
//         Variable* v = new Variable(parameter->sexps[0]->text, parameter->sexps[0]->lineno);
//         SCM_ASSERT(v);
//         variables->add(v);
//         Object* value;
//         SExp* p1 = parameter->sexps[1];
//         int ret = translate(&p1, &value);
//         if (ret != SUCCESS) return ret;
//         values->add(value);
//     }

//     Objects* body = new Objects;SCM_ASSERT(body);
//     for (int i = 3; i < L(); i++)
//     {
//         Object* o;
//         SExp* n1 = N(i);
//         int ret = translate(&n1, &o);
//         if (ret != SUCCESS) return ret;
//         body->add(o);
//     }
//     *object = new NamedLet(body, variables, values, N(1)->text, sexp->lineno);SCM_ASSERT(*object);
//     return SUCCESS;
// }

// int Translator::translateLet(SExp* sexp, Object** object)
// {
//     if (L() < 3)
//     {
//         printf("%s %s:%d L() = %d\n", __func__, __FILE__, __LINE__, L());fflush(stdout);// debug
//         return SYNTAX_ERROR;
//     }
//     if (N(1)->type == SExp::SYMBOL)
//     {
//         return translateNamedLet(sexp, object);
//     }
//     if (N(1)->type != SExp::SEXPS)
//     {
//         printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
//         return SYNTAX_ERROR;
//     }

//     Variables* variables = new Variables;SCM_ASSERT(variables);
//     Objects* values = new Objects;SCM_ASSERT(values);
//     SExps* parameterSExps = &N(1)->sexps;
//     for (int i = 0; i < parameterSExps->size(); i++)
//     {
//         SExp* parameter = parameterSExps->get(i);
//         if (parameter->type != SExp::SEXPS || parameter->sexps.size() != 2)
//         {
//             printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
//             return SYNTAX_ERROR;
//         }
//         if (parameter->sexps[0]->type != SExp::SYMBOL)
//         {
//             printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
//             return SYNTAX_ERROR;
//         }
//         Variable* v = new Variable(parameter->sexps[0]->text, parameter->sexps[0]->lineno);
//         SCM_ASSERT(v);
//         variables->add(v);
//         Object* value;
//         SExp* p1 = parameter->sexps[1];
//         int ret = translate(&p1, &value);
//         if (ret != SUCCESS)
//         {
//             printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
//             return ret;
//         }
//         values->add(value);
//     }

//     Objects* body = new Objects;SCM_ASSERT(body);
//     for (int i = 2; i < L(); i++)
//     {
//         Object* o;
//         SExp* ni = N(i);
//         int ret = translate(&ni, &o);
//         if (ret != SUCCESS)
//         {
//             printf("i = %d\n", i);
//             printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
//             return ret;
//         }
//         body->add(o);
//     }
//     *object = new Let(body, variables, values, sexp->lineno);SCM_ASSERT(*object);
//     return SUCCESS;
// }

// int Translator::translateLetAsterisk(SExp* sexp, Object** object)
// {
//     if (L() < 3) return SYNTAX_ERROR;
//     if (N(1)->type != SExp::SEXPS) return SYNTAX_ERROR;

//     Variables* variables = new Variables;SCM_ASSERT(variables);
//     Objects* values = new Objects;SCM_ASSERT(values);
//     SExps* parameterSExps = &N(1)->sexps;
//     for (int i = 0; i < parameterSExps->size(); i++)
//     {
//         SExp* parameter = parameterSExps->get(i);
//         if (parameter->type != SExp::SEXPS || parameter->sexps.size() != 2) return SYNTAX_ERROR;
//         if (parameter->sexps[0]->type != SExp::SYMBOL) return SYNTAX_ERROR;
//         Variable* v = new Variable(parameter->sexps[0]->text, parameter->sexps[0]->lineno);
//         SCM_ASSERT(v);
//         variables->add(v);
//         Object* value;
//         SExp* p1 = parameter->sexps[1];
//         int ret = translate(&p1, &value);
//         if (ret != SUCCESS) return ret;
//         values->add(value);
//     }

//     Objects* body = new Objects;SCM_ASSERT(body);
//     for (int i = 2; i < L(); i++)
//     {
//         Object* o;
//         SExp* ni = N(i);
//         int ret = translate(&ni, &o);
//         if (ret != SUCCESS) return ret;
//         body->add(o);
//     }
//     *object = new LetAsterisk(body, variables, values, sexp->lineno);SCM_ASSERT(*object);
//     return SUCCESS;
// }

// int Translator::translateApplication(SExp* sexp, Object** object)
// {
//     Object* f;
//     SExp* n0 = N(0);
//     int ret = translate(&n0, &f);
//     if (ret != SUCCESS) return ret;
//     Objects* arguments = new Objects;SCM_ASSERT(arguments);
//     for (int i = 1; i < L(); i++)
//     {
//         Object * object;
//         SExp* ni = N(i);
//         int ret = translate(&ni, &object);
//         if (ret != SUCCESS) return ret;
//         arguments->add(object);
//     }
//     *object = new Application(f, arguments, sexp->lineno);SCM_ASSERT(*object);
//     return SUCCESS;
// }

// int Translator::translate(SExp** n, Object** object)
// {
//     SExp* sexp = *n;
//     if (sexp->type != SExp::SEXPS)
//     {
//         return translatePrimitive(sexp, object);
//     }

//     if (L() <= 0) return SYNTAX_ERROR;

//     SExp* function = N(0);
//     if (function->type == SExp::SYMBOL)
//     {
//         String functionName = function->text;

//         if (functionName == "define")
//         {
//             return translateDefinition(sexp, object);
//         }
//         else if (functionName == "define-macro")
//         {
//             return translateDefinitionMacro(sexp, object);
//         }
//         else if (functionName == "defmacro")
//         {
//             return translateDefinitionMacro(sexp, object);
//         }
//         else if (functionName == "define-syntax")
//         {
//             // fix me
//             return SUCCESS;
//         }
//         else if (functionName == "if")
//         {
//             return translateIf(sexp, object);
//         }
//         else if (functionName == "begin")
//         {
//             return translateBegin(sexp, object);
//         }
//         else if (functionName == "lambda")
//         {
//             return translateLambda(sexp, object);
//         }
//         else if (functionName == "quote")
//         {
//             return translateQuote(sexp, object);
//         }
//         else if (functionName == "quasiquote")
//         {
//             return translateQuasiQuote(sexp, object);
//         }
//         else if (functionName == "unquote")
//         {
//             return translateUnquote(sexp, object);
//         }
// #if 0
//         else if (functionName == "and")
//         {
//             return translateAnd(sexp, object);
//         }
//         else if (functionName == "or")
//         {
//             return translateOr(sexp, object);
//         }
// #endif
// #if 0
//         else if (functionName == "cond")
//         {
//             return translateCond(sexp, object);
//         }
// #endif
//         else if (functionName == "let")
//         {
//             return translateLet(sexp, object);
//         }
// #if 1
//         else if (functionName == "let*")
//         {
//             return translateLetAsterisk(sexp, object);
//         }
// #endif
//         else
//         {
//             return translateApplication(sexp, object);
//         }
//     }
//     else
//     {
//         return translateApplication(sexp, object);
//     }
//     return SYNTAX_ERROR;
// }

// // we use and/or macro now
// # if 0
// int Translator::translateAnd(SExp* sexp, Object** object)
// {
//     Objects* objects = new Objects;SCM_ASSERT(objects);
//     for (int i = 1; i < L(); i++)
//     {
//         Object * object;
//         int ret = translate(&N(i), &object);
//         if (ret != SUCCESS) return ret;
//         objects->add(object);
//     }
//     *object = __A(new And(objects));SCM_ASSERT(*object);
//     return SUCCESS;
// }


// int Translator::translateOr(SExp* sexp, Object** object)
// {
//     Objects* objects = new Objects;SCM_ASSERT(objects);
//     for (int i = 1; i < L(); i++)
//     {
//         Object * object;
//         int ret = translate(&N(i), &object);
//         if (ret != SUCCESS) return ret;
//         objects->add(object);
//     }
//     *object = __A(new Or(objects));SCM_ASSERT(*object);
//     return SUCCESS;
// }

// int Translator::translateDefineSyntax(SExp* sexp)
// {
//     if (L() != 3) return SYNTAX_ERROR;
//     if (!N(1)->isSymbol()) return SYNTAX_ERROR;
//     if (!N(2)->isSExps() || LL(2) < 3) return SYNTAX_ERROR;
//     if (!NN(2, 0)->isSymbol() || NN(2, 0)->text != "syntax-rules") return SYNTAX_ERROR;
//     if (!NN(2, 1)->isSExps()) return SYNTAX_ERROR;
//     // macro name
//     Macro* macro = new Macro(N(1)->text);

//     // store reserved words
//     for (SExps::const_iterator p = NN(2, 1)->sexps.begin(); p != NN(2, 1)->sexps.end(); ++p)
//     {
//         SExp* n = (*p);
//         if (!n->isSymbol()) return SYNTAX_ERROR;
//         macro->reservedWords.add(n->text);
//     }
//     // store pattern / definition
//     for (int i = 2; i < LL(2); ++i)
//     {
//         SExp* n = NN(2, i);
//         if (!n->isSExps() || n->sexps.size() != 2) return SYNTAX_ERROR;
//         macro->addPattern(n->sexps[0], n->sexps[1]);
//     }
// //    macros_[macro->name] = macro;
//     return SUCCESS;
// }
// #endif
