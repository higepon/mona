#include "Eval.h"

using namespace monash;

Eval::Eval(Translator& translator, Quote* quote, uint32_t lineno /* = 0 */) : translator_(translator_), quote_(quote), lineno_(lineno)
{
}

Eval::~Eval()
{
}

std::string Eval::toString()
{
    return "eval:";
}

int Eval::type() const
{
    return Object::EVAL;
}

Object* Eval::eval(Environment* env)
{
    Object* o;
    // see Translator::translateQuote
    SExp* n = quote_->sexp();
    env->macroFilter().filter(n);
//    n->print();
    int ret = translator_.translate(&n, &o);
    if (ret != Translator::SUCCESS)
    {
        RAISE_ERROR(n->lineno, "eval got error [%s]", quote_->toStringValue().c_str());
    }
    return o->eval(env);
}
