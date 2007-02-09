#ifndef __SCHEME_H__
#define __SCHEME_H__

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <vector>


namespace std {
    typedef vector<string> strings;
};

#include <algorithm>
#include "MacroFilter.h"
#include "QuoteFilter.h"
#include "Object.h"
#include "Environment.h"
#include "Procedure.h"
#include "Number.h"
#include "String.h"
#include "Continuation.h"
#include "Charcter.h"
#include "Quote.h"
#include "Vector.h"
#include "Pair.h"
#include "SpecialIf.h"
#include "Assignment.h"
#include "Begin.h"
#include "Definition.h"
#include "Application.h"
#include "SExp.h"
#include "Parser.h"
#include "Translator.h"
#include "Macro.h"
#include "Error.h"
#include "OutputPort.h"
#include "InputPort.h"
#include "procedures/True.h"
#include "procedures/False.h"
#include "procedures/Undef.h"
#include "procedures/Procedure.h"
#include "procedures/Set.h"

std::string load(const char* file);
void registerPrimitives(monash::Environment* env);

#endif // __SCHEME_H__
