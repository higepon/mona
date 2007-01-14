// #ifndef __PRIMITIVE_PROCEDURE_PAIR_PROCEDURES_H__
// #define __PRIMITIVE_PROCEDURE_PAIR_PROCEDURES_H__

// PROCEDURE(Cons, "cons")
// {
//     ARGC_SHOULD_BE(2);
//     return new Pair(ARGV(0), ARGV(1));
// }

// PROCEDURE(Car, "car")
// {
//     ARGC_SHOULD_BE(1);
//     if (ARGV(0)->isQuote())
//     {
//         Quote* quote = (Quote*)ARGV(0);
//         Quote* ret = quote->car();
//         if (ret == NULL)
//         {
//             RAISE_ERROR(quote->lineno(), "%s got error on quote %s", procedureName_.c_str(), quote->toString().c_str());
//         }
//         return ret;
//     }
//     else if (ARGV(0)->isPair())
//     {
//         Pair* p = (Pair*)ARGV(0);
//         return p->first();
//     }
//     RAISE_ERROR(ARGV(0)->lineno(), "car got [%s], but required pair", procedureName_.c_str(), ARGV(0)->toString().c_str());
//     return NULL;
// }

// PROCEDURE(Cdr, "cdr")
// {
//     ARGC_SHOULD_BE(1);
//     Object* o = ARGV(0);
//     if (o->isQuote())
//     {
//         Quote* quote = (Quote*)o;
//         Quote* ret = quote->cdr();
//         if (ret == NULL)
//         {
//             RAISE_ERROR(o->lineno(), "%s got error on quote %s", procedureName_.c_str(), o->toString().c_str());
//         }
//         return ret;
//     }
//     else if (o->isPair())
//     {
//         Pair* p = (Pair*)o;
//         return p->second();
//     }
//     RAISE_ERROR(o->lineno(), "%s got [%s], but required pair", procedureName_.c_str(), o->toString().c_str());
//     return NULL;
// }

// #endif // __PRIMITIVE_PROCEDURE_PAIR_PROCEDURES_H__
