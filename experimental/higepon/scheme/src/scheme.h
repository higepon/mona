#ifndef __SCHEME_H__
#define __SCHEME_H__

#include <string>
#include <vector>
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
#include "Node.h"
#include "Parser.h"
#include "Translator.h"

monash::Object* eval(monash::Object* exp, monash::Environment* env);
monash::Object* evalSequence(monash::Objects* exps, monash::Environment* env);
monash::Objects* listOfValues(monash::Objects* objects, monash::Environment* env);
monash::Object* apply(monash::Object* procedure, monash::Objects* arguments);
bool isTrue(monash::Object* exp);
std::string load(const char* file);
void setInputText(const std::string& text);
void display(monash::Object* exp);

#endif // __SCHEME_H__
