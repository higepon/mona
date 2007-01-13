#include "Divide.h"

using namespace monash;

// Divide::Divide()
// {
// }

// Divide::~Divide()
// {
// }

// std::string Divide::toString()
// {
//     return "procedure:+";
// }

// Object* Divide::eval(Environment* env)
// {
//     RAISE_ERROR(lineno(), "don't eval procedure [%s]", toString().c_str());
//     return NULL;
// }

// Object* Divide::apply(Objects* arguments, Environment* env)
// {
//     Objects* as = listOfValues(arguments, env);
//     if (as->size() == 0 || as->at(0)->type() != Object::NUMBER)
//     {
//         RAISE_ERROR(as->size() >= 0 ? as->at(0)->lineno() : 0, "devide got wrong arguments")
//         return NULL;
//     }
//     Number* n = (Number*)as->at(0);

//     int total = n->value();
//     for (Objects::const_iterator it = as->begin() + 1; it != as->end(); it++)
//     {
//         Object* o = (*it);
//         if (o->type() == Object::NUMBER)
//         {
//             Number* nn = (Number*)o;
//             if (nn->value() == 0)
//             {
//                 RAISE_ERROR(nn->lineno(), "devide by zero")
//                 return NULL;
//             }
//             total /= nn->value();
//         }
//     }
//     return new Number(total);
// }
