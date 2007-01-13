#include "CharcterP.h"

using namespace monash;

using namespace std;

// CharcterP::CharcterP()
// {
// }

// CharcterP::~CharcterP()
// {
// }

// std::string CharcterP::toString()
// {
//     return "procedure:charcterp";
// }

// Object* CharcterP::eval(Environment* env)
// {
//     RAISE_ERROR(lineno(), "don't eval procedure [%s]", toString().c_str());
//     return NULL;
// }

// Object* CharcterP::apply(Objects* arguments, Environment* env)
// {
//     Objects* as = listOfValues(arguments, env);

//     if (as->size() != 0)
//     {
//         RAISE_ERROR(as->size() >= 0 ? as->at(0)->lineno() : 0, "charcterp got %d arguments, but required %d", as->size(), 0);
//         return NULL;
//     }
//     RETURN_BOOLEAN(as->at(0)->isCharcter());
// }


