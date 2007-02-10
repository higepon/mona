#include "scheme.h"

using namespace std;
using namespace monash;

DefaultProcedures procedures;

OutputPort* g_currentOutputPort;
OutputPort* g_defaultOutputPort;
InputPort* g_currentInputPort;
InputPort* g_defaultInputPort;

void registerPrimitives(Environment* env)
{
    g_defaultOutputPort = new OutputPort(stdout);
    g_defaultInputPort  = new InputPort(stdin);
    g_currentInputPort  = g_defaultInputPort;
    g_currentOutputPort = g_defaultOutputPort;

    for (DefaultProcedures::const_iterator p = procedures.begin(); p != procedures.end(); ++p)
    {
        env->defineVariable((*p).first, (*p).second);
    }
    env->defineVariable(new Variable("#f"),             new False());
    env->defineVariable(new Variable("#t"),             new True());
    env->defineVariable(new Variable("set!"),           new Set());
}
