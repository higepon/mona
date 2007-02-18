#include "scheme.h"

using namespace util;
using namespace monash;

DefaultProcedures procedures;

OutputPort* g_currentOutputPort;
OutputPort* g_defaultOutputPort;
InputPort* g_currentInputPort;
InputPort* g_defaultInputPort;
FILE* g_transcript;

void registerPrimitives(Environment* env)
{
    g_defaultOutputPort = new OutputPort(stdout);
    g_defaultInputPort  = new InputPort(stdin);
    g_currentInputPort  = g_defaultInputPort;
    g_currentOutputPort = g_defaultOutputPort;
    g_transcript = NULL;
    for (int i = 0; i < procedures.size(); i++)
    {
        env->defineVariable(procedures[i].first, procedures[i].second);
    }
    env->defineVariable(new Variable("#f"),             new False());
    env->defineVariable(new Variable("#t"),             new True());
    env->defineVariable(new Variable("set!"),           new Set());
}
