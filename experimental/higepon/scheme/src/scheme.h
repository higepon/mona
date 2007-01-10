#ifndef __SCHEME_H__
#define __SCHEME_H__

#include <string>
#include <vector>

namespace std {
    typedef vector<string> strings;
};

#include <string>
#include <vector>
#include "MacroFilter.h"
#include "Object.h"
#include "Environment.h"
#include "Procedure.h"
#include "Number.h"
#include "String.h"
#include "Quote.h"
#include "SpecialIf.h"
#include "Assignment.h"
#include "Begin.h"
#include "Definition.h"
#include "Application.h"
#include "SExp.h"
#include "Eval.h"
#include "Parser.h"
#include "Translator.h"
#include "Macro.h"
#include "Error.h"
#include "PrimitiveProcedures/Plus.h"
#include "PrimitiveProcedures/Minus.h"
#include "PrimitiveProcedures/Divide.h"
#include "PrimitiveProcedures/Multiply.h"
#include "PrimitiveProcedures/Car.h"
#include "PrimitiveProcedures/Cdr.h"
#include "PrimitiveProcedures/Cons.h"
#include "PrimitiveProcedures/Display.h"
#include "PrimitiveProcedures/NumberP.h"
#include "PrimitiveProcedures/StringP.h"
#include "PrimitiveProcedures/StringToNumber.h"
#include "PrimitiveProcedures/StringAppend.h"
#include "PrimitiveProcedures/NumberEqual.h"
#include "PrimitiveProcedures/Set.h"
#include "PrimitiveProcedures/Eqv.h"
#include "PrimitiveProcedures/True.h"
#include "PrimitiveProcedures/False.h"
#include "PrimitiveProcedures/Load.h"


monash::Object* eval(monash::Object* exp, monash::Environment* env);
monash::Object* evalSequence(monash::Objects* exps, monash::Environment* env);
monash::Objects* listOfValues(monash::Objects* objects, monash::Environment* env);
monash::Object* apply(monash::Object* procedure, monash::Objects* arguments, monash::Environment* env);
std::string load(const char* file);
void setInputText(const std::string& text);
void display(monash::Object* exp);

void registerPrimitives(monash::Environment* env);

#endif // __SCHEME_H__
