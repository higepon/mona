#define GLOBAL_VALUE_DEFINED
#include "scheme.h"

using namespace util;
using namespace monash;

#include "primitive_procedures.h"

void registerPrimitives(Environment* env)
{
    g_defaultOutputPort = new OutputPort(stdout);
    g_defaultInputPort  = new InputPort(stdin);
    g_currentInputPort  = g_defaultInputPort;
    g_currentOutputPort = g_defaultOutputPort;
    g_transcript = NULL;
    g_dynamic_winds = new DynamicWinds;
    g_true = new True;
    g_false = new False;
    g_undef = new Undef;
    g_no_arg = new Objects;

#include "register.inc"


//     for (int i = 0; i < procedures.size(); i++)
//     {
//         env->defineVariable(procedures[i].first, procedures[i].second);
//     }
    env->defineVariable(new Variable("#f"),             g_false);
    env->defineVariable(new Variable("#t"),             g_true);
    env->defineVariable(new Variable("set!"),           new Set());
}
