#ifndef __SCHEME_H__
#define __SCHEME_H__

#ifdef GLOBAL_VALUE_DEFINED
#define GLOBAL /* */
#define GLOBAL_VAL(v) = (v)
#else
#define GLOBAL extern "C"
#define GLOBAL_VAL(v) /* */
#endif


#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "util/Vector.h"
#include "util/String.h"
#include "util/Pair.h"
#include "util/HashMap.h"

namespace util {
    typedef Vector<String> Strings;
};

//#include <algorithm>
#include "MacroFilter.h"
#include "QuoteFilter.h"
#include "Object.h"
#include "Environment.h"
#include "Procedure.h"
#include "Number.h"
#include "SString.h"
#include "Charcter.h"
#include "Quote.h"
#include "Vector.h"
#include "Pair.h"
#include "SpecialIf.h"
#include "Assignment.h"
#include "Begin.h"
#include "Definition.h"
#include "Application.h"
#include "Continuation.h"
#include "SExp.h"
#include "Parser.h"
#include "Translator.h"
#include "Macro.h"
#include "Error.h"
#include "OutputPort.h"
#include "InputPort.h"
#include "Values.h"
#include "procedures/True.h"
#include "procedures/False.h"
#include "procedures/Undef.h"
#include "procedures/Procedure.h"
#include "procedures/Set.h"

::util::String load(const char* file);
void registerPrimitives(monash::Environment* env);

class monash::InputPort;
namespace monash {
class DynamicWind;
typedef ::util::Vector<DynamicWind*> DynamicWinds;
typedef ::util::Vector< ::util::Pair<Variable*, Object*> > DefaultProcedures;

}
GLOBAL monash::True* g_true;
GLOBAL monash::False* g_false;
GLOBAL monash::Undef* g_undef;
GLOBAL monash::Objects* g_no_arg;
GLOBAL monash::DynamicWinds* g_dynamic_winds;
GLOBAL monash::InputPort* g_defaultInputPort;
GLOBAL monash::DefaultProcedures procedures;
GLOBAL monash::OutputPort* g_currentOutputPort;
GLOBAL monash::OutputPort* g_defaultOutputPort;
GLOBAL monash::InputPort* g_currentInputPort;
GLOBAL bool g_gc_initialized GLOBAL_VAL(false);

GLOBAL FILE* g_transcript GLOBAL_VAL(NULL);

#define SCM_TRUE   g_true
#define SCM_FALSE  g_false
#define SCM_UNDEF  g_undef
#define SCM_NO_ARG g_no_arg

// notice!
// don't use like below
// SCHEME_WRITE(stream, o->eval);
// o->eval will be called twice!
#define SCHEME_WRITE(stream, ...)                                       \
{                                                                       \
    fprintf(stream, __VA_ARGS__);                                       \
    int __file = fileno(stream);                                        \
    int __out  = fileno(stdout);                                        \
    int __err  = fileno(stderr);                                        \
    if (g_transcript != NULL && (__file == __out || __file == __err))   \
    {                                                                   \
        fprintf(g_transcript, __VA_ARGS__);                             \
    }                                                                   \
}

#define TRANSCRIPT_WRITE(...)                                   \
{                                                               \
    if (g_transcript != NULL)                                   \
    {                                                           \
        fprintf(g_transcript, __VA_ARGS__);                     \
    }                                                           \
}

#endif // __SCHEME_H__
