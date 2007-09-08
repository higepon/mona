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
        FOREACH_LIST(formals, car, cdr, "lambda")
        {
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
    FOREACH_LIST(cons->getCdr(), car , cdr, "cond")
    {
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
    FOREACH_LIST(c, car, cdr, syntax.data())
    {
        body->add(translate(car));
        if (cdr->isNil())
        {
            break;
        }
    }
}

// for <let> and <named let>
void Translator::translateBindingSpec(Object* bindingSpecs, Variables* bindings, Cons* specs)
{
    Cons* currentSpecs = specs;
    if (bindingSpecs->isCons())
    {
        FOREACH_LIST(bindingSpecs, car, cdr, "let or named let")
        {
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

            if (cdr->isNil())
            {
                break;
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
            FOREACH_LIST(kdr, car, cdr, "define")
            {
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

