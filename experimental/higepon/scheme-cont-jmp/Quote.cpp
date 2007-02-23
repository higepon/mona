#include "Quote.h"

using namespace util;
using namespace monash;

Quote::Quote(SExp* sexp, uint32_t lineno /* = 0 */) : sexp_(sexp), lineno_(lineno)
{
}

Quote::~Quote()
{
}

String Quote::toString()
{
    return "quote: \'";
}

String Quote::toStringValue()
{
    return sexp_->toSExpString();
}

int Quote::type() const
{
    return Object::QUOTE;
}

Quote* Quote::car()
{
    if (sexp_->isSExps())
    {
        return new Quote(sexp_->sexps[0], lineno_);
    }
    return NULL;
}

Quote* Quote::cdr()
{
    if (sexp_->isSExps())
    {
        SExp* n = new SExp(SExp::SEXPS);
        for (int i = 1; i < sexp_->sexps.size(); i++)
        {
            n->sexps.add(sexp_->sexps[i]);
        }
        return new Quote(n, lineno_);
    }
    return NULL;
}

Object* Quote::eval(Environment* env)
{
    if (sexp_->isString())
    {
        if (sexp_->text == "#t")
        {
            return SCM_TRUE;
        }
        else if (sexp_->text == "#f")
        {
            return g_false;
        }
        else
        {
            return new SString(sexp_->text, sexp_->lineno);
        }
    }
    else if (sexp_->isNumber())
    {
        return new Number(sexp_->value, sexp_->lineno);
    }
    else
    {
        return this;
    }
}

bool Quote::eqv(Object* o)
{
    if (!o->isQuote()) return false;
    Quote* quote = (Quote*)o;
    return this->toStringValue() == quote->toStringValue();
}

bool Quote::eq(Object* o)
{
    return eqv(o);
}
