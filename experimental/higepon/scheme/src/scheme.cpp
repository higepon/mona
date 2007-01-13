#include "scheme.h"

using namespace std;
using namespace monash;

Object* evalSequence(Objects* exps, Environment* env)
{
    Object* ret = NULL;
    for (Objects::const_iterator it = exps->begin(); it != exps->end(); ++it)
    {
        Object* o = (*it);
        ret = o->eval(env);
    }
    return ret;
}

Objects* listOfValues(Objects* objects, Environment* env)
{
    Objects* result = new Objects;ASSERT(result);
    for (Objects::const_iterator it = objects->begin(); it != objects->end(); ++it)
    {
        result->push_back((*it)->eval(env));
    }
    return result;
}

bool isCompoundProcedure(Object* p)
{
    return p->type() == Object::PROCEDURE;
}

bool isPrimitiveProcedure(Object* p)
{
    return p->type() == Object::PRIMITIVE_PROCEDURE;
}

Object* apply(Object* procedure, Objects* arguments, Environment* env)
{
    if (isCompoundProcedure(procedure))
    {
       Objects* as = listOfValues(arguments, env);
        Procedure* p = (Procedure*)procedure;
        Environment* e = p->env()->clone();

        e->extend(p->parameters(), as); // doubt? we need copy?
        return evalSequence(p->body(), e);
    }
    else if (isPrimitiveProcedure(procedure))
    {
        PrimitiveProcedure* p = (PrimitiveProcedure*)procedure;
        return p->apply(arguments, env);
    }
    else
    {
        RAISE_ERROR(procedure->lineno(), "unknown procedure [%s]", procedure->toString().c_str());
        return NULL;
    }
}

bool isTrue(Object* exp)
{
    return exp->type() != Object::FALSE;
}

void display(Object* exp)
{
    printf("%s", exp->toString().c_str());
}

PROCEDURE_BEGIN(CharcterEqualP, char=?)
{
    ARGC_SHOULD_BE(2);
    SCHEME_CAST(ARGV(0), Charcter, c);
    RETURN_BOOLEAN(c->eqv(ARGV(1)));
}
PROCEDURE_END

PROCEDURE_BEGIN(CharcterGtP, char>?)
{
    ARGC_SHOULD_BE(2);
    SCHEME_CAST(ARGV(0), Charcter, c);
    RETURN_BOOLEAN(c->gt(ARGV(1)));
}
PROCEDURE_END

PROCEDURE_BEGIN(CharcterLtP, char<?)
{
    ARGC_SHOULD_BE(2);
    SCHEME_CAST(ARGV(0), Charcter, c);
    RETURN_BOOLEAN(c->lt(ARGV(1)));
}
PROCEDURE_END

PROCEDURE_BEGIN(CharcterGeP, char>=?)
{
    ARGC_SHOULD_BE(2);
    SCHEME_CAST(ARGV(0), Charcter, c);
    RETURN_BOOLEAN(c->ge(ARGV(1)));
}
PROCEDURE_END

PROCEDURE_BEGIN(CharcterLeP, char<=?)
{
    ARGC_SHOULD_BE(2);
    SCHEME_CAST(ARGV(0), Charcter, c);
    RETURN_BOOLEAN(c->le(ARGV(1)));
}
PROCEDURE_END

PROCEDURE_BEGIN(CharcterToInteger, char->integer)
{
    ARGC_SHOULD_BE(1);
    SCHEME_CAST(ARGV(0), Charcter, c);
    return c->toNumber();
}
PROCEDURE_END

PROCEDURE_BEGIN(IntegerToCharcter,integer->char)
{
    ARGC_SHOULD_BE(1);
    SCHEME_CAST(ARGV(0), Number, n);
    return Charcter::fromNumber(n);
}
PROCEDURE_END

PROCEDURE_BEGIN(VectorP, vector?)
{
    ARGC_SHOULD_BE(1);
    RETURN_BOOLEAN(ARGV(0)->isVector());
}
PROCEDURE_END

PROCEDURE_BEGIN(MakeVector, make-vector)
{
    Objects* as = listOfValues(arguments, env);
    if (ARGC != 1 && ARGC != 2)
    {
        RAISE_ERROR(ARGC >= 0 ? ARGV(0)->lineno() : 0, "%s got %d arguments, but required 1 or 2", toString().c_str(), ARGC);
        return NULL;
    }

    if (ARGC == 1)
    {
        SCHEME_CAST(ARGV(0), Number, n)
        return new Vector(n->value(), n->lineno());
    }
    else
    {
        SCHEME_CAST(ARGV(0), Number, n)
        return new Vector(n->value(), ARGV(1), n->lineno());
    }
}
PROCEDURE_END

PROCEDURE_BEGIN(CreateVector, vector)
{
    Objects* as = listOfValues(arguments, env);
    return new Vector(as, as->empty() ? 0 : ARGV(0)->lineno());
}
PROCEDURE_END

PROCEDURE_BEGIN(VectorLength, vector-length)
{
    ARGC_SHOULD_BE(1);
    SCHEME_CAST(ARGV(0), Vector, v);
    return new Number(v->size(), v->lineno());
}
PROCEDURE_END

PROCEDURE_BEGIN(VectorRef, vector-ref)
{
    ARGC_SHOULD_BE(2);
    SCHEME_CAST(ARGV(0), Vector, v);
    SCHEME_CAST(ARGV(1), Number, n);
    Object* ret = v->get(n->value());
    if (ret == NULL)
    {
        RAISE_ERROR(ARGV(0)->lineno(), "%s got wrong index", toString().c_str());
        return NULL;
    }
    return ret;
}
PROCEDURE_END

PROCEDURE_BEGIN(VectorSet, vector-set)
{
    ARGC_SHOULD_BE(3);
    SCHEME_CAST(ARGV(0), Vector, v);
    SCHEME_CAST(ARGV(1), Number, n);
    bool isOK = v->set(n->value(), ARGV(2));
    if (!isOK)
    {
        RAISE_ERROR(ARGV(0)->lineno(), "%s got wrong arguments", toString().c_str());
    }
    return new True();
}
PROCEDURE_END

PROCEDURE_BEGIN(StringEqualP, string=?)
{
    ARGC_SHOULD_BE(2);
    SCHEME_CAST(ARGV(0), String, s);
    RETURN_BOOLEAN(s->equal(ARGV(1)));
}
PROCEDURE_END

PROCEDURE_BEGIN(SymbolP, symbol?)
{
    ARGC_SHOULD_BE(1);
    RETURN_BOOLEAN(ARGV(0)->isQuote());
}
PROCEDURE_END

PROCEDURE_BEGIN(SymbolToString, symbol->string)
{
    ARGC_SHOULD_BE(1);
    SCHEME_CAST(ARGV(0), Quote, q);
    return new String(q->toStringValue(), q->lineno());
}
PROCEDURE_END


//PROCEDURE_DECLARE(CharcterEqualP)
//PROCEDURE_DECLARE(CharcterGtP)
// PROCEDURE_DECLARE(CharcterLtP)
// PROCEDURE_DECLARE(CharcterGeP)
// PROCEDURE_DECLARE(CharcterLeP)
// PROCEDURE_DECLARE(CharcterToInteger)
// PROCEDURE_DECLARE(IntegerToCharcter)
// PROCEDURE_DECLARE(VectorP)
// PROCEDURE_DECLARE(MakeVector)
// PROCEDURE_DECLARE(CreateVector)
// PROCEDURE_DECLARE(VectorLength)
// PROCEDURE_DECLARE(VectorRef)
// PROCEDURE_DECLARE(VectorSet)
// PROCEDURE_DECLARE(StringEqualP)
// PROCEDURE_DECLARE(SymbolP)
// PROCEDURE_DECLARE(SymbolToString)


// PROCEDURE_BODY(CharcterEqualP, arguments, env)
// {
//     ARGC_SHOULD_BE(2);
//     SCHEME_CAST(ARGV(0), Charcter, c);
//     RETURN_BOOLEAN(c->eqv(ARGV(1)));
// }


// PROCEDURE_BODY(CharcterGtP, arguments, env)
// {
//     ARGC_SHOULD_BE(2);
//     SCHEME_CAST(ARGV(0), Charcter, c);
//     RETURN_BOOLEAN(c->gt(ARGV(1)));
// }

// PROCEDURE_BODY(CharcterLtP, arguments, env)
// {
//     ARGC_SHOULD_BE(2);
//     SCHEME_CAST(ARGV(0), Charcter, c);
//     RETURN_BOOLEAN(c->lt(ARGV(1)));
// }

// PROCEDURE_BODY(CharcterGeP, arguments, env)
// {
//     ARGC_SHOULD_BE(2);
//     SCHEME_CAST(ARGV(0), Charcter, c);
//     RETURN_BOOLEAN(c->ge(ARGV(1)));
// }

// PROCEDURE_BODY(CharcterLeP, arguments, env)
// {
//     ARGC_SHOULD_BE(2);
//     SCHEME_CAST(ARGV(0), Charcter, c);
//     RETURN_BOOLEAN(c->le(ARGV(1)));
// }

// PROCEDURE_BODY(CharcterToInteger, arguments, env)
// {
//     ARGC_SHOULD_BE(1);
//     SCHEME_CAST(ARGV(0), Charcter, c);
//     return c->toNumber();
// }

// PROCEDURE_BODY(IntegerToCharcter, arguments, env)
// {
//     ARGC_SHOULD_BE(1);
//     SCHEME_CAST(ARGV(0), Number, n);
//     return Charcter::fromNumber(n);
// }

// PROCEDURE_BODY(VectorP, arguments, env)
// {
//     ARGC_SHOULD_BE(1);
//     RETURN_BOOLEAN(ARGV(0)->isVector());
// }

// PROCEDURE_BODY(MakeVector, arguments, env)
// {
//     Objects* as = listOfValues(arguments, env);
//     if (ARGC != 1 && ARGC != 2)
//     {
//         RAISE_ERROR(ARGC >= 0 ? ARGV(0)->lineno() : 0, "%s got %d arguments, but required 1 or 2", toString().c_str(), ARGC);
//         return NULL;
//     }

//     if (ARGC == 1)
//     {
//         SCHEME_CAST(ARGV(0), Number, n)
//         return new Vector(n->value(), n->lineno());
//     }
//     else
//     {
//         SCHEME_CAST(ARGV(0), Number, n)
//         return new Vector(n->value(), ARGV(1), n->lineno());
//     }
// }

// PROCEDURE_BODY(CreateVector, arguments, env)
// {
//     Objects* as = listOfValues(arguments, env);
//     return new Vector(as, as->empty() ? 0 : ARGV(0)->lineno());
// }

// PROCEDURE_BODY(VectorLength, arguments, env)
// {
//     ARGC_SHOULD_BE(1);
//     SCHEME_CAST(ARGV(0), Vector, v);
//     return new Number(v->size(), v->lineno());
// }

// PROCEDURE_BODY(VectorRef, arguments, env)
// {
//     ARGC_SHOULD_BE(2);
//     SCHEME_CAST(ARGV(0), Vector, v);
//     SCHEME_CAST(ARGV(1), Number, n);
//     Object* ret = v->get(n->value());
//     if (ret == NULL)
//     {
//         RAISE_ERROR(ARGV(0)->lineno(), "%s got wrong index", toString().c_str());
//         return NULL;
//     }
//     return ret;
// }

// PROCEDURE_BODY(VectorSet, arguments, env)
// {
//     ARGC_SHOULD_BE(3);
//     SCHEME_CAST(ARGV(0), Vector, v);
//     SCHEME_CAST(ARGV(1), Number, n);
//     bool isOK = v->set(n->value(), ARGV(2));
//     if (!isOK)
//     {
//         RAISE_ERROR(ARGV(0)->lineno(), "%s got wrong arguments", toString().c_str());
//     }
//     return new True();
// }

// PROCEDURE_BODY(StringEqualP, arguments, env)
// {
//     ARGC_SHOULD_BE(2);
//     SCHEME_CAST(ARGV(0), String, s);
//     RETURN_BOOLEAN(s->equal(ARGV(1)));
// }

// PROCEDURE_BODY(SymbolP, arguments, env)
// {
//     ARGC_SHOULD_BE(1);
//     RETURN_BOOLEAN(ARGV(0)->isQuote());
// }

// PROCEDURE_BODY(SymbolToString, arguments, env)
// {
//     ARGC_SHOULD_BE(1);
//     SCHEME_CAST(ARGV(0), Quote, q);
//     return new String(q->toStringValue(), q->lineno());
// }


void registerPrimitives(Environment* env)
{
    env->defineVariable(new Variable("+"),              new Plus());
    env->defineVariable(new Variable("-"),              new Minus());
    env->defineVariable(new Variable("*"),              new Multiply());
    env->defineVariable(new Variable("/"),              new Divide());
    env->defineVariable(new Variable("="),              new NumberEqual());
    env->defineVariable(new Variable("cons"),           new Cons());
    env->defineVariable(new Variable("car"),            new Car());
    env->defineVariable(new Variable("cdr"),            new Cdr());
    env->defineVariable(new Variable("display"),        new Display());
    env->defineVariable(new Variable("string?"),        new StringP());
    env->defineVariable(new Variable("char=?"),         new CharcterEqualP());
    env->defineVariable(new Variable("char>?"),         new CharcterGtP());
    env->defineVariable(new Variable("char<?"),         new CharcterLtP());
    env->defineVariable(new Variable("char>=?"),        new CharcterGeP());
    env->defineVariable(new Variable("char<=?"),        new CharcterLeP());
    env->defineVariable(new Variable("char->integer"),  new CharcterToInteger());
    env->defineVariable(new Variable("integer->char"),  new IntegerToCharcter());

    env->defineVariable(new Variable("eqv?"),           new Eqv());
    env->defineVariable(new Variable("number?"),        new NumberP());
    env->defineVariable(new Variable("string->number"), new StringToNumber());
    env->defineVariable(new Variable("string-append"),  new StringAppend());
    env->defineVariable(new Variable("#f"),             new False());
    env->defineVariable(new Variable("#t"),             new True());
    env->defineVariable(new Variable("set!"),           new Set());
    env->defineVariable(new Variable("eqv?"),           new Eqv());


    env->defineVariable(new Variable("vector?"),         new VectorP());
    env->defineVariable(new Variable("make-vector"),     new MakeVector());
    env->defineVariable(new Variable("vector"),           new CreateVector());
    env->defineVariable(new Variable("vector-length"),    new VectorLength());
    env->defineVariable(new Variable("vector-ref"),       new VectorRef());
    env->defineVariable(new Variable("vector-set!"),      new VectorSet());
    env->defineVariable(new Variable("string=?"),      new StringEqualP());
    env->defineVariable(new Variable("symbol?"),      new SymbolP());
    env->defineVariable(new Variable("symbol->string"),      new SymbolToString());

// load
    env->defineVariable(new Variable("load"),           new Load());
}
