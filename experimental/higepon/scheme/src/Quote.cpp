#include "Quote.h"

using namespace monash;

Quote::Quote(SExp* sexp, uint32_t lineno /* = 0 */) : sexp_(sexp), lineno_(lineno)
{
}

Quote::~Quote()
{
}

std::string Quote::toString()
{
    return "quote: \'";
}

std::string Quote::toStringValue()
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
        for (SExps::const_iterator p = sexp_->sexps.begin() + 1; p != sexp_->sexps.end(); ++p)
        {
            n->sexps.push_back(*p);
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
            return new True();
        }
        else if (sexp_->text == "#f")
        {
            return new False();
        }
        else
        {
            return new String(sexp_->text, sexp_->lineno);
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
