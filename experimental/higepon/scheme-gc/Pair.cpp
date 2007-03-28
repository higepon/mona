#include "Pair.h"

//using namespace util;
using namespace monash;

Pair::Pair(Object* car, Object* cdr, uint32_t lineno) : car_(car), cdr_(cdr), lineno_(lineno)
{
}

Pair::~Pair()
{
}

// SExp* Pair::objectToSExp(Object* o)
// {
//     SExp* sexp;
//     if (o->isNumber())
//     {
//         sexp = new SExp(SExp::NUMBER, lineno());
//         Number* n = (Number*)o;
//         sexp->value = n->value();
//     }
//     else if (o->isNil())
//     {
//         sexp = new SExp(SExp::SEXPS, lineno());
// //        SExp* quoteSexp = new SExp(SExp::SYMBOL);
//         //       quoteSexp->text = "quote";
// //        sexp->sexps.add(quoteSexp);
// //        sexp->sexps.add(new SExp(SExp::SEXPS));
//     }
//     else if (o->isCharcter())
//     {
//         sexp = new SExp(SExp::CHAR, lineno());
//         Charcter* c = (Charcter*)o;
//         sexp->text = c->stringValue();

//     }
//     else if (o->isSString())
//     {
//         sexp = new SExp(SExp::STRING, lineno());
//         SString* s = (SString*)o;
//         sexp->text = s->value();
//     }
//     else if (o->isVariable())
//     {
//         sexp = new SExp(SExp::SYMBOL, lineno());
//         Variable* v = (Variable*)o;
//         sexp->text = v->name();
//     }
//     else if (o->isRiteralConstant())
//     {
//         sexp = new SExp(SExp::SYMBOL, lineno());
//         RiteralConstant* r = (RiteralConstant*)o;
// //        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
//         sexp->text = r->text();
//     }
//     else if (o->isPair())
//     {
//         Pair* p = (Pair*)o;
//         sexp = p->toSExp();
//     }
//     else if (o->isTrue())
//     {
//         sexp = new SExp(SExp::SYMBOL, lineno());
//         sexp->text = "#t";
//     }
//     else if (o->isFalse())
//     {
//         sexp = new SExp(SExp::SYMBOL, lineno());
//         sexp->text = "#f";
//     }
//     else
//     {
//         RAISE_ERROR(lineno(), "objectToSExp error %s\n", o->typeString().data());
//     }
//     return sexp;
// }

// SExp* Pair::toSExp()
// {
//     SExp* sexp = new SExp(SExp::SEXPS);
//     Object* o = this;
//     for (;;)
//     {
//         if (o->isNil())
//         {
//             break;
//         }
//         else if (o->isPair())
//         {
//             Pair* p = (Pair*)o;
//             sexp->sexps.add(objectToSExp(p->getCar()));
//             o = p->getCdr();
//         }
//         else
//         {
//             sexp->sexps.add(objectToSExp(o));
//             break;
//         }
//     }
// //    printf("result=%s\n", sexp->toSExpString().data());
//     return sexp;
// }

::util::String Pair::toString()
{
    return toStringInternal(false);
}

::util::String Pair::toStringInternal(bool inList /* = false */)
{
    ::util::String carString = car_ ? car_->toString() : "NULL";
    ::util::String cdrString = cdr_ ? cdr_->toString() : "NULL";
    if (cdr_->isPair())
    {
        Pair* p = (Pair*)cdr_;
        if (inList)
        {
            return carString + " " + p->toStringInternal(true);
        }
        else
        {
            if (carString == "quote")
            {
                return "\'" + p->toStringInternal(true);
            }
            else
            {
                return "(" + carString + " " + p->toStringInternal(true) + ")";
            }
        }
    }
    else if (cdr_->isNil())
    {
        return carString;
    }
    else
    {
        return "(" + carString + " . " + cdrString + ")";
    }
}


::util::String Pair::toStringValue()
{
    ::util::String carString = car_ ? car_->toStringValue() : "NULL";
    ::util::String cdrString = cdr_ ? cdr_->toStringValue() : "NULL";
    return "(cons " + carString + " " + cdrString + ")";
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
