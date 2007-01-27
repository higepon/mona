#include "scheme.h"

using namespace std;
using namespace monash;

DefaultProcedures procedures;

void registerPrimitives(Environment* env)
{
    for (DefaultProcedures::const_iterator p = procedures.begin(); p != procedures.end(); ++p)
    {
        env->defineVariable((*p).first, (*p).second);
    }
    env->defineVariable(new Variable("#f"),             new False());
    env->defineVariable(new Variable("#t"),             new True());
    env->defineVariable(new Variable("set!"),           new Set());
}
