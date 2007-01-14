// #include "Display.h"

// using namespace monash;

// Display::Display()
// {
// }

// Display::~Display()
// {
// }

// std::string Display::toString()
// {
//     return "procedure:display";
// }

// Object* Display::eval(Environment* env)
// {
//     RAISE_ERROR(lineno(), "don't eval procedure [%s]", toString().c_str());
//     return NULL;
// }

// Object* Display::apply(Objects* arguments, Environment* env)
// {
//     Objects* as = listOfValues(arguments, env);
//     if (as->size() != 1)
//     {
//         RAISE_ERROR(as->size() >= 0 ? as->at(0)->lineno() : 0, "display got %d arguments, but required %d", as->size(), 1);
//         return NULL;
//     }
//     printf(as->at(0)->toStringValue().c_str());
//     return new Number(0); // hutei
// }
