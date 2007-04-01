#include "Translator.h"

using namespace util;
using namespace monash;
#define SYNTAX_ERROR(...) printf(__VA_ARGS__);fflush(stdout);
// #define N(n)         (sexp->sexps[n])
// #define NN(i, j)     sexp->sexps[i]->sexps[j]
// #define NNN(i, j, k) sexp->sexps[i]->sexps[j]->sexps[k]
// #define L()          sexp->sexps.size()
// #define LL(n)        sexp->sexps[n]->sexps.size()

Translator::Translator() : inLambda_(false)
{
}

Translator::~Translator()
{
}

Object* Translator::translate2(Object* data)
{
    if (data->isRiteralConstant())
    {
        RiteralConstant* r = (RiteralConstant*)data;
        return new Variable(r->text());
    }
    else
    {
        return data;
    }

    return NULL;
}

Object* Translator::translateBegin2(Pair* data)
{
    Object* ocar = data->getCar();
    if (!ocar->isRiteralConstant())
    {
        SYNTAX_ERROR("begin");
        return NULL;
    }
    RiteralConstant* r = (RiteralConstant*)ocar;
    if (r->text() != "begin")
    {
        SYNTAX_ERROR("begin");
        return NULL;
    }
    Objects* body = new Objects;
    Pair* pair;
    for (Object* p = data->getCdr(); !p->isNil();)
    {
        if (!p->isPair())
        {
            SYNTAX_ERROR("begin");
            return NULL;
        }
        pair = (Pair*)p;
        body->add(translate2(pair->getCar()));
        p = pair->getCdr();
    }
    return new Begin(body);

}

// change from externel representation to Scheme objects
int Translator::translateAsData(SExp* sexp, Object** object)
{
    if (sexp->type != SExp::SEXPS)
    {
        return translateAsDataPrimitive(sexp, object);
    }

    if (L() > 1 && N(0)->text == "VECTOR")
    {
        return translateAsVectorData(sexp, object);
    }
    else
    {
        if (L() > 0 && N(0)->text == "lambda") inLambda_ = true;
        int ret = translateAsListData(sexp, object);
        if (inLambda_) inLambda_ = false;
        return ret;
    }
}

int Translator::translateAsVectorData(SExp* sexp, Object** object)
{
    ASSERT(L() > 0);
    ASSERT(N(0)->text == "VECTOR");
    Vector* v = new Vector(L() - 1, sexp->lineno);
    for (int i = 1; i < L(); i++)
    {
        Object* o;
        if (translateAsData(N(i), &o) != SUCCESS) return SYNTAX_ERROR;
        v->set(i - 1, o);
    }
    *object = v;
    return SUCCESS;
}

int Translator::translateAsListData(SExp* sexp, Object** object)
{
    // ()
    if (L() == 0)
    {
        *object = SCM_NIL;
        return SUCCESS;
    }

    // (a . b)
    if (!inLambda_ && L() == 3 && N(1)->isSymbol() && N(1)->text == ".")
    {
        Object* car;
        Object* cdr;
        if (translateAsData(N(0), &car) != SUCCESS) return SYNTAX_ERROR;
        if (translateAsData(N(2), &cdr) != SUCCESS) return SYNTAX_ERROR;
        *object = new Pair(car, cdr, sexp->lineno);
        return SUCCESS;
    }

    // (a b c d)
    Pair* start = new Pair(SCM_NIL, SCM_NIL, sexp->lineno);
    Pair* p = start;
    for (int i = 0; i < L(); i++)
    {
        Object* car;
        if (translateAsData(N(i), &car) != SUCCESS) return SYNTAX_ERROR;
        p->setCar(car);
        if (i != L() -1)
        {
            Pair* tmp = new Pair(SCM_NIL, SCM_NIL, sexp->lineno);
            p->setCdr(tmp);
            p = tmp;
        }
    }
    *object = start;
    return SUCCESS;
}

int Translator::translateAsDataPrimitive(SExp* sexp, Object** object)
{
    switch(sexp->type)
    {
    case SExp::NUMBER:
#ifdef USE_MONA_GC
        *object = new(false) Number(sexp->value, sexp->lineno);ASSERT(*object);
#else
        *object = new Number(sexp->value, sexp->lineno);ASSERT(*object);
#endif
        return SUCCESS;
    case SExp::STRING:
        *object = new SString(sexp->text, sexp->lineno);ASSERT(*object);
        return SUCCESS;
    case SExp::CHAR:
#ifdef USE_MONA_GC
        *object = new(false) Charcter(sexp->text, sexp->lineno);ASSERT(*object);
#else
        *object = new Charcter(sexp->text, sexp->lineno);ASSERT(*object);
#endif
        return SUCCESS;
    case SExp::SYMBOL:
        if (sexp->text == "#t")
        {
            *object = SCM_TRUE;
        }
        else if (sexp->text == "#f")
        {
            *object = SCM_FALSE;
        }
        else
        {
            *object = new RiteralConstant(sexp->text, sexp->lineno);ASSERT(*object);
        }
        return SUCCESS;
    }
    return SYNTAX_ERROR;
}


int Translator::translatePrimitive(SExp* sexp, Object** object)
{
    switch(sexp->type)
    {
    case SExp::NUMBER:
#ifdef USE_MONA_GC
        *object = new(false) Number(sexp->value, sexp->lineno);ASSERT(*object);
#else
        *object = new Number(sexp->value, sexp->lineno);ASSERT(*object);
#endif
        return SUCCESS;
    case SExp::STRING:
#ifdef USE_MONA_GC
        *object = new SString(sexp->text, sexp->lineno);ASSERT(*object);
#else
        *object = new SString(sexp->text, sexp->lineno);ASSERT(*object);
#endif
        return SUCCESS;
//     case SExp::QUOTE:
//         printf("quote:%s\n", sexp->text.data());
//         *object = new Quote(SExp::fromString(sexp->text), sexp->lineno);ASSERT(*object);
//         return SUCCESS;
    case SExp::CHAR:
#ifdef USE_MONA_GC
        *object = new(false) Charcter(sexp->text, sexp->lineno);ASSERT(*object);
#else
        *object = new Charcter(sexp->text, sexp->lineno);ASSERT(*object);
#endif
        return SUCCESS;
    case SExp::SYMBOL:
#ifdef USE_MONA_GC
        *object = new Variable(sexp->text, sexp->lineno);ASSERT(*object);
#else
        *object = new Variable(sexp->text, sexp->lineno);ASSERT(*object);
#endif
        return SUCCESS;
    }
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    return SYNTAX_ERROR;
}

int Translator::translateDefinition(SExp* sexp, Object** object)
{
#if 0
    if (L() != 3) return SYNTAX_ERROR;
    SExp* symbol = N(1);
    if (symbol->type != SExp::SYMBOL) return SYNTAX_ERROR;
    Variable* variable = new Variable(symbol->text, symbol->lineno);ASSERT(variable);
    SExp* argument = N(2);
    Object* argumentObject;
    if (translate(&argument, &argumentObject) != SUCCESS) return SYNTAX_ERROR;
    *object = new Definition(variable, argumentObject, sexp->lineno);ASSERT(*object);
    return SUCCESS;
#else
    if (N(1)->isSExps())
    {
        Variable* variable = new Variable(NN(1, 0)->text, NN(1, 0)->lineno);ASSERT(variable);
        Variables* variables = new Variables;
        for (int i = 1; i < LL(1); i++)
        {
            Variable* v = new Variable(NN(1, i)->text, NN(1, i)->lineno);ASSERT(v);
            variables->add(v);
        }
        Objects* body = new Objects;
        for (int i = 2; i < L(); i++)
        {
            Object* b;
            SExp* s = N(i);
            if (translate(&s, &b) != SUCCESS) {printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);return SYNTAX_ERROR;}
            body->add(b);
        }
        Object* l = new Lambda(body, variables, false, sexp->lineno);ASSERT(l);
        *object = new Definition(variable, l, sexp->lineno);ASSERT(*object);
        return SUCCESS;
    }
    else
    {
        if (L() != 3) {printf("%s %s:%d L()=%d %s %s\n", __func__, __FILE__, __LINE__, L(), N(0)->toSExpString().data(), N(3)->toSExpString().data());fflush(stdout);return SYNTAX_ERROR;}
        SExp* symbol = N(1);
        if (symbol->type != SExp::SYMBOL) {printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);return SYNTAX_ERROR;}
        Variable* variable = new Variable(symbol->text, symbol->lineno);ASSERT(variable);
        SExp* argument = N(2);
        Object* argumentObject;
        if (translate(&argument, &argumentObject) != SUCCESS) {printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);return SYNTAX_ERROR;}
        *object = new Definition(variable, argumentObject, sexp->lineno);ASSERT(*object);
        return SUCCESS;
    }
#endif
}

int Translator::translateIf(SExp* sexp, Object** object)
{
    if (L() > 4 || L() < 3) return SYNTAX_ERROR;
    Object* predicate = NULL;
    Object* consequent = NULL;
    Object* alternative = NULL;
    SExp* n1 = N(1);
    int ret = translate(&n1, &predicate);
    if (ret != SUCCESS) return ret;
    SExp* n2 = N(2);
    ret = translate(&n2, &consequent);
    if (ret != SUCCESS) return ret;
    if (L() == 4)
    {
        SExp* n3 = N(3);
        ret = translate(&n3, &alternative);
        if (ret != SUCCESS) return ret;
    }
    *object = new SpecialIf(predicate, consequent, alternative, sexp->lineno);ASSERT(*object);
    return SUCCESS;
}

int Translator::translateCond(SExp* sexp, Object** object)
{
    Clauses* clauses = new Clauses;ASSERT(clauses);
    Objects* elseActions = NULL;
    for (int i = 1; i < L(); i++)
    {
        SExp* n = sexp->sexps[i];
        if (n->sexps.size() < 2) return SYNTAX_ERROR;
        if (i == L() - 1 && n->sexps[0]->type == SExp::SYMBOL && n->sexps[0]->text == "else")
        {
            elseActions = new Objects;ASSERT(elseActions);
            for (int j = 1; j < n->sexps.size(); j++)
            {
                Object * action;
                SExp* nj = n->sexps[j];
                int ret = translate(&nj, &action);
                if (ret != SUCCESS) return ret;
                elseActions->add(action);
            }
        }
        else
        {
            // (cond (1 => hoge))
            if (n->sexps.size() == 3 && n->sexps[1]->type == SExp::SYMBOL && n->sexps[1]->text == "=>")
            {
                Object* cond;
                SExp* n0 = n->sexps[0];
                int ret = translate(&n0, &cond);
                if (ret != SUCCESS) return ret;
                Object* action;
                SExp* n2 = n->sexps[2];
                ret = translate(&n2, &action);
                if (ret != SUCCESS) return ret;
                Objects* arguments = new Objects;ASSERT(arguments);
                arguments->add(cond);
                Object* application = new Application(action, arguments, action->lineno());ASSERT(application);
                Objects* actions = new Objects;ASSERT(actions);
                actions->add(application);
                Clause* c = new Clause(cond, actions);
                ASSERT(c);
                clauses->add(c);
            }
            else
            {
                Object* cond;
                SExp* n0 = n->sexps[0];
                int ret = translate(&n0, &cond);
                if (ret != SUCCESS) return ret;
                Objects* actions = new Objects;ASSERT(actions);
                for (int j = 1; j < n->sexps.size(); j++)
                {
                    Object * action;
                    SExp* nj = n->sexps[j];
                    ret = translate(&nj, &action);
                    if (ret != SUCCESS) return ret;
                    actions->add(action);
                }
                Clause* c = new Clause(cond, actions);
                ASSERT(c);
                clauses->add(c);
            }
        }
    }
    *object = new Cond(clauses, elseActions, sexp->lineno);ASSERT(*object);
    return SUCCESS;
}

int Translator::translateBegin(SExp* sexp, Object** object)
{
    if (L() <= 1) return SYNTAX_ERROR;
    Objects* objects = new Objects;ASSERT(objects);
    for (int i = 1; i < L(); i++)
    {
        Object* object;
        SExp* ni = N(i);
        int ret = translate(&ni, &object);
        if (ret != SUCCESS) return ret;
        objects->add(object);
    }
    *object = new Begin(objects, sexp->lineno);ASSERT(*object);
    return SUCCESS;
}

int Translator::translateQuote(SExp* sexp, Object** object)
{
    if (L() < 1)
    {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        return SYNTAX_ERROR;
    }
//    *object = new Quote(sexp->sexps[1], sexp->lineno);ASSERT(*object);
    translateAsData(sexp->sexps[1], object);
    return SUCCESS;
}

int Translator::translateLambda(SExp* sexp, Object** object)
{
    if (L() <= 2)
    {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        return SYNTAX_ERROR;
    }
    if (N(1)->type != SExp::SEXPS && N(1)->type != SExp::SYMBOL)
    {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        return SYNTAX_ERROR;
    }
    bool extendVariable = false;
    Variables* variables = new Variables;ASSERT(variables);
    if (N(1)->type == SExp::SEXPS)
    {

        for (int i = 0; i < N(1)->sexps.size(); i++)
        {
            SExp* param = NN(1, i);
            if (param->type != SExp::SYMBOL)
            {
                printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
                return SYNTAX_ERROR;
            }
            Variable* v = new Variable(param->text, param->lineno);
            ASSERT(v);
            variables->add(v);
        }
    }
    else if (N(1)->type == SExp::SYMBOL)
    {
        Variable* v = new Variable(N(1)->text, N(1)->lineno);
        extendVariable = true;
        variables->add(v);
    }
    Objects* body = new Objects;ASSERT(body);
    for (int i = 2; i < L(); i++)
    {
        Object* o;
        SExp* ni = N(i);
        int ret = translate(&ni, &o);
        if (ret != SUCCESS)
        {
            printf("%s %s:%d:i=%d\n", __func__, __FILE__, __LINE__, i);fflush(stdout);// debug
            return ret;
        }
        body->add(o);
    }
    *object = new Lambda(body, variables, extendVariable, sexp->lineno);ASSERT(*object);
    return SUCCESS;
}

int Translator::translateNamedLet(SExp* sexp, Object** object)
{
    if (N(2)->type != SExp::SEXPS)
    {
        return SYNTAX_ERROR;
    }
    Variables* variables = new Variables;ASSERT(variables);
    Objects* values = new Objects;ASSERT(values);
    SExps* parameterSExps = &N(2)->sexps;
    for (int i = 0; i < parameterSExps->size(); i++)
    {
        SExp* parameter = parameterSExps->get(i);
        if (parameter->type != SExp::SEXPS || parameter->sexps.size() != 2) return SYNTAX_ERROR;
        if (parameter->sexps[0]->type != SExp::SYMBOL) return SYNTAX_ERROR;
        Variable* v = new Variable(parameter->sexps[0]->text, parameter->sexps[0]->lineno);
        ASSERT(v);
        variables->add(v);
        Object* value;
        SExp* p1 = parameter->sexps[1];
        int ret = translate(&p1, &value);
        if (ret != SUCCESS) return ret;
        values->add(value);
    }

    Objects* body = new Objects;ASSERT(body);
    for (int i = 3; i < L(); i++)
    {
        Object* o;
        SExp* n1 = N(i);
        int ret = translate(&n1, &o);
        if (ret != SUCCESS) return ret;
        body->add(o);
    }
    *object = new NamedLet(body, variables, values, N(1)->text, sexp->lineno);ASSERT(*object);
    return SUCCESS;
}

int Translator::translateLet(SExp* sexp, Object** object)
{
    if (L() < 3)
    {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        return SYNTAX_ERROR;
    }
    if (N(1)->type == SExp::SYMBOL)
    {
        return translateNamedLet(sexp, object);
    }
    if (N(1)->type != SExp::SEXPS)
    {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        return SYNTAX_ERROR;
    }

    Variables* variables = new Variables;ASSERT(variables);
    Objects* values = new Objects;ASSERT(values);
    SExps* parameterSExps = &N(1)->sexps;
    for (int i = 0; i < parameterSExps->size(); i++)
    {
        SExp* parameter = parameterSExps->get(i);
        if (parameter->type != SExp::SEXPS || parameter->sexps.size() != 2)
        {
            printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
            return SYNTAX_ERROR;
        }
        if (parameter->sexps[0]->type != SExp::SYMBOL)
        {
            printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
            return SYNTAX_ERROR;
        }
        Variable* v = new Variable(parameter->sexps[0]->text, parameter->sexps[0]->lineno);
        ASSERT(v);
        variables->add(v);
        Object* value;
        SExp* p1 = parameter->sexps[1];
        int ret = translate(&p1, &value);
        if (ret != SUCCESS)
        {
            printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
            return ret;
        }
        values->add(value);
    }

    Objects* body = new Objects;ASSERT(body);
    for (int i = 2; i < L(); i++)
    {
        Object* o;
        SExp* ni = N(i);
        int ret = translate(&ni, &o);
        if (ret != SUCCESS)
        {
            printf("i = %d\n", i);
            printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
            return ret;
        }
        body->add(o);
    }
    *object = new Let(body, variables, values, sexp->lineno);ASSERT(*object);
    return SUCCESS;
}

int Translator::translateLetAsterisk(SExp* sexp, Object** object)
{
    if (L() < 3) return SYNTAX_ERROR;
    if (N(1)->type != SExp::SEXPS) return SYNTAX_ERROR;

    Variables* variables = new Variables;ASSERT(variables);
    Objects* values = new Objects;ASSERT(values);
    SExps* parameterSExps = &N(1)->sexps;
    for (int i = 0; i < parameterSExps->size(); i++)
    {
        SExp* parameter = parameterSExps->get(i);
        if (parameter->type != SExp::SEXPS || parameter->sexps.size() != 2) return SYNTAX_ERROR;
        if (parameter->sexps[0]->type != SExp::SYMBOL) return SYNTAX_ERROR;
        Variable* v = new Variable(parameter->sexps[0]->text, parameter->sexps[0]->lineno);
        ASSERT(v);
        variables->add(v);
        Object* value;
        SExp* p1 = parameter->sexps[1];
        int ret = translate(&p1, &value);
        if (ret != SUCCESS) return ret;
        values->add(value);
    }

    Objects* body = new Objects;ASSERT(body);
    for (int i = 2; i < L(); i++)
    {
        Object* o;
        SExp* ni = N(i);
        int ret = translate(&ni, &o);
        if (ret != SUCCESS) return ret;
        body->add(o);
    }
    *object = new LetAsterisk(body, variables, values, sexp->lineno);ASSERT(*object);
    return SUCCESS;
}

int Translator::translateApplication(SExp* sexp, Object** object)
{
    Object* f;
    SExp* n0 = N(0);
    int ret = translate(&n0, &f);
    if (ret != SUCCESS) return ret;
    Objects* arguments = new Objects;ASSERT(arguments);
    for (int i = 1; i < L(); i++)
    {
        Object * object;
        SExp* ni = N(i);
        int ret = translate(&ni, &object);
        if (ret != SUCCESS) return ret;
        arguments->add(object);
    }
    *object = new Application(f, arguments, sexp->lineno);ASSERT(*object);
    return SUCCESS;
}

int Translator::translate(SExp** n, Object** object)
{
    SExp* sexp = *n;
    if (sexp->type != SExp::SEXPS)
    {
        return translatePrimitive(sexp, object);
    }

    if (L() <= 0) return SYNTAX_ERROR;

    SExp* function = N(0);
    if (function->type == SExp::SYMBOL)
    {
        String functionName = function->text;

        if (functionName == "define")
        {
            return translateDefinition(sexp, object);
        }
        else if (functionName == "define-syntax")
        {
            // fix me
            return SUCCESS;
        }
        else if (functionName == "if")
        {
            return translateIf(sexp, object);
        }
        else if (functionName == "begin")
        {
            return translateBegin(sexp, object);
        }
        else if (functionName == "lambda")
        {
            return translateLambda(sexp, object);
        }
        else if (functionName == "quote")
        {
            return translateQuote(sexp, object);
        }
#if 0
        else if (functionName == "and")
        {
            return translateAnd(sexp, object);
        }
        else if (functionName == "or")
        {
            return translateOr(sexp, object);
        }
#endif
#if 0
        else if (functionName == "cond")
        {
            return translateCond(sexp, object);
        }
#endif
        else if (functionName == "let")
        {
            return translateLet(sexp, object);
        }
#if 1
        else if (functionName == "let*")
        {
            return translateLetAsterisk(sexp, object);
        }
#endif
        else
        {
            return translateApplication(sexp, object);
        }
    }
    else
    {
        return translateApplication(sexp, object);
    }
    return SYNTAX_ERROR;
}

// we use and/or macro now
# if 0
int Translator::translateAnd(SExp* sexp, Object** object)
{
    Objects* objects = new Objects;ASSERT(objects);
    for (int i = 1; i < L(); i++)
    {
        Object * object;
        int ret = translate(&N(i), &object);
        if (ret != SUCCESS) return ret;
        objects->add(object);
    }
    *object = __A(new And(objects));ASSERT(*object);
    return SUCCESS;
}


int Translator::translateOr(SExp* sexp, Object** object)
{
    Objects* objects = new Objects;ASSERT(objects);
    for (int i = 1; i < L(); i++)
    {
        Object * object;
        int ret = translate(&N(i), &object);
        if (ret != SUCCESS) return ret;
        objects->add(object);
    }
    *object = __A(new Or(objects));ASSERT(*object);
    return SUCCESS;
}

int Translator::translateDefineSyntax(SExp* sexp)
{
    if (L() != 3) return SYNTAX_ERROR;
    if (!N(1)->isSymbol()) return SYNTAX_ERROR;
    if (!N(2)->isSExps() || LL(2) < 3) return SYNTAX_ERROR;
    if (!NN(2, 0)->isSymbol() || NN(2, 0)->text != "syntax-rules") return SYNTAX_ERROR;
    if (!NN(2, 1)->isSExps()) return SYNTAX_ERROR;
    // macro name
    Macro* macro = new Macro(N(1)->text);

    // store reserved words
    for (SExps::const_iterator p = NN(2, 1)->sexps.begin(); p != NN(2, 1)->sexps.end(); ++p)
    {
        SExp* n = (*p);
        if (!n->isSymbol()) return SYNTAX_ERROR;
        macro->reservedWords.add(n->text);
    }
    // store pattern / definition
    for (int i = 2; i < LL(2); ++i)
    {
        SExp* n = NN(2, i);
        if (!n->isSExps() || n->sexps.size() != 2) return SYNTAX_ERROR;
        macro->addPattern(n->sexps[0], n->sexps[1]);
    }
//    macros_[macro->name] = macro;
    return SUCCESS;
}
#endif
