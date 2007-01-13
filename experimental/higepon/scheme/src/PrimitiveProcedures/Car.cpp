// #include "Car.h"

// using namespace monash;

// Car::Car()
// {
// }

// Car::~Car()
// {
// }

// std::string Car::toString()
// {
//     return "procedure:car";
// }

// Object* Car::eval(Environment* env)
// {
//     RAISE_ERROR(lineno(), "don't eval procedure [%s]", toString().c_str());
//     return NULL;
// }

// Object* Car::apply(Objects* arguments, Environment* env)
// {
//     Objects* as = listOfValues(arguments, env);

//     if (as->size() != 1)
//     {
//         RAISE_ERROR(as->size() >= 0 ? as->at(0)->lineno() : 0, "car got %d arguments, but required %d", as->size(), 1);
//         return NULL;
//     }
//     Object* o = as->at(0);
//     if (o->type() == Object::QUOTE)
//     {
//         Quote* quote = (Quote*)o;
//         Quote* ret = quote->car();
//         if (ret == NULL)
//         {
//             RAISE_ERROR(o->lineno(), "car got error on quote %s", o->toString().c_str());
//         }
//         return ret;
//     }
//     else if (o->type() == Object::PAIR)
//     {
//         Pair* p = (Pair*)o;
//         return p->first();
//     }
//     RAISE_ERROR(o->lineno(), "car got [%s], but required pair", o->toString().c_str());
//     return NULL;
// }
