#include "Quote.h"

using namespace util;
using namespace monash;

Quote::Quote(SExp* sexp, uint32_t lineno /* = 0 */) : sexp_(sexp), data_(NULL), lineno_(lineno)
{
}

Quote::~Quote()
{
}

String Quote::toString()
{
    return toStringValue();//"quote: \'";
}

String Quote::toStringValue()
{
    return sexp_->toSExpString();
}

int Quote::type() const
{
    return Object::QUOTE;
}

// Quote* Quote::car()
// {
//     if (sexp_->isSExps())
//     {
//         return new Quote(sexp_->sexps[0], lineno_);
//     }
//     return NULL;
// }

// Quote* Quote::cdr()
// {
//     if (sexp_->isSExps())
//     {
//         SExp* n = new SExp(SExp::SEXPS);
//         for (int i = 1; i < sexp_->sexps.size(); i++)
//         {
//             n->sexps.add(sexp_->sexps[i]);
//         }
//         return new Quote(n, lineno_);
//     }
//     return NULL;
// }

Object* Quote::eval(Environment* env)
{
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    if (NULL == data_)
    {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        if (env->translator().translateAsData(sexp_, &data_) != Translator::SUCCESS)
        {
            RAISE_ERROR(lineno(), "quote error\n");
        }
    }
    return data_;

#if 0
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
#ifdef USE_MONA_GC
            return new SString(sexp_->text, sexp_->lineno);
#else
            return new SString(sexp_->text, sexp_->lineno);
#endif
        }
    }
    else if (sexp_->isNumber())
    {
#ifdef USE_MONA_GC
        return new(false) Number(sexp_->value, sexp_->lineno);
#else
        return new Number(sexp_->value, sexp_->lineno);
#endif
    }
    else
    {
        return this;
    }
#endif
}

bool Quote::eqv(Object* o)
{
    if (!o->isQuote()) return false;
    Quote* quote = (Quote*)o;
    printf("eqv%s %s\n", this->toStringValue().data(),quote->toStringValue().data());
    return this->toStringValue() == quote->toStringValue();
}

bool Quote::eq(Object* o)
{
    return eqv(o);
}
