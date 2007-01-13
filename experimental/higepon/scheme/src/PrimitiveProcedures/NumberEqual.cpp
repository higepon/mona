#include "NumberEqual.h"

using namespace monash;

// NumberEqual::NumberEqual()
// {
// }

// NumberEqual::~NumberEqual()
// {
// }

// std::string NumberEqual::toString()
// {
//     return "procedure:=";
// }

// Object* NumberEqual::eval(Environment* env)
// {
//     RAISE_ERROR(lineno(), "don't eval procedure [%s]", toString().c_str());
//     return NULL;
// }

// Object* NumberEqual::apply(Objects* arguments, Environment* env)
// {
//     Objects* as = listOfValues(arguments, env);
//     if (as->size() != 2) return new False();
//     Object* o1 = as->at(0);
//     Object* o2 = as->at(1);
//     if (o1->type() != Object::NUMBER || o2->type() != Object::NUMBER) return new False();
//     Number* n1 = (Number*)o1;
//     Number* n2 = (Number*)o2;
//     if (n1->value() == n2->value()) return new True();
//     return new False();
// }
