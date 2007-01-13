#include "NumberP.h"

using namespace monash;

// NumberP::NumberP()
// {
// }

// NumberP::~NumberP()
// {
// }

// std::string NumberP::toString()
// {
//     return "procedure:number?";
// }

// Object* NumberP::eval(Environment* env)
// {
//     RAISE_ERROR(lineno(), "don't eval procedure [%s]", toString().c_str());
//     return NULL;
// }

// Object* NumberP::apply(Objects* arguments, Environment* env)
// {
//     Objects* as = listOfValues(arguments, env);
//     if (as->size() != 1)
//     {
//         RAISE_ERROR(as->size() >= 0 ? as->at(0)->lineno() : 0, "number? got %d arguments, but required %d", as->size(), 1);
//         return NULL;
//     }
//     Object* o = as->at(0);
//     if (o->type() == Object::NUMBER)
//     {
//         return new True();
//     }
//     else
//     {
//         return new False();
//     }
// }
