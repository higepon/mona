#include "Plus.h"

using namespace monash;

// Plus::Plus()
// {
// }

// Plus::~Plus()
// {
// }

// std::string Plus::toString()
// {
//     return "procedure:+";
// }

// Object* Plus::eval(Environment* env)
// {
//     RAISE_ERROR(lineno(), "don't eval procedure [%s]", toString().c_str());
//     return NULL;
// }

// Object* Plus::apply(Objects* arguments, Environment* env)
// {
//     Objects* as = listOfValues(arguments, env);
//     int total = 0;
//     for (Objects::const_iterator it = as->begin(); it != as->end(); it++)
//     {
//         Object* o = (*it);
//         if (o->type() == Object::NUMBER)
//         {
//             Number* n = (Number*)o;
//             total += n->value();
//         }
//     }
//     return new Number(total);
// }
