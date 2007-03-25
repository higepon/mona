#include "Pair.h"

//using namespace util;
using namespace monash;

Pair::Pair(Object* car, Object* cdr, uint32_t lineno) : car_(car), cdr_(cdr), lineno_(lineno)
{
}

Pair::~Pair()
{
}

SExp* Pair::objectToSExp(Object* o)
{
    SExp* sexp;
    if (o->isNumber())
    {
        sexp = new SExp(SExp::NUMBER);
        Number* n = (Number*)o;
        sexp->value = n->value();
    }
    else if (o->isNil())
    {
        sexp = new SExp(SExp::SEXPS);
        SExp* quoteSexp = new SExp(SExp::SYMBOL);
        quoteSexp->text = "quote";
        sexp->sexps.add(quoteSexp);
        sexp->sexps.add(new SExp(SExp::SEXPS));
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    }
    else if (o->isSString())
    {
        sexp = new SExp(SExp::STRING);
        SString* s = (SString*)o;
        sexp->text = s->value();
    }
    else if (o->isVariable())
    {
        sexp = new SExp(SExp::SYMBOL);
        Variable* v = (Variable*)o;
        sexp->text = v->name();
    }
    else if (o->isRiteralConstant())
    {
        sexp = new SExp(SExp::SYMBOL);
        RiteralConstant* r = (RiteralConstant*)o;
        sexp->text = r->text();
    }
    else if (o->isPair())
    {
        Pair* p = (Pair*)o;
        sexp = p->toSExp();
    }
    else
    {
        RAISE_ERROR(lineno(), "objectToSExp error %s\n", o->typeString().data());
    }
    return sexp;
}

SExp* Pair::toSExp()
{
    SExp* sexp = new SExp(SExp::SEXPS);
    Object* o = this;
    for (;;)
    {
        if (o == SCM_NIL) break;
        if (o->isPair())
        {
            Pair* p = (Pair*)o;
            if (p->getCar() == SCM_NIL) break;

            sexp->sexps.add(objectToSExp(p->getCar()));
            o = p->getCdr();
        }
        else
        {
            sexp->sexps.add(objectToSExp(o));
            break;
        }
    }
    return sexp;
}

::util::String Pair::toString()
{
    ::util::String carString = car_ ? car_->toStringValue() : "NULL";
    ::util::String cdrString = cdr_ ? cdr_->toStringValue() : "NULL";
    return "pair(" + carString + ", " + cdrString + ")";
}

::util::String Pair::toStringValue()
{
    ::util::String carString = car_ ? car_->toStringValue() : "NULL";
    ::util::String cdrString = cdr_ ? cdr_->toStringValue() : "NULL";
    return "(" + carString + ", " + cdrString + ")";
}

int Pair::type() const
{
    return Object::PAIR;
}

Object* Pair::eval(Environment* env)
{
    return this;
}

bool Pair::eqv(Object* o)
{
    if (!o->isPair()) return false;
    Pair* p = (Pair*)o;
    return this == p;
}

bool Pair::eq(Object* o)
{
    return eqv(o);
}

void Pair::setCar(Object* o)
{
    car_ = o;
}

void Pair::setCdr(Object* o)
{
    cdr_ = o;
}
