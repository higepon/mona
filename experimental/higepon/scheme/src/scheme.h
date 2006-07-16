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

monash::Object* eval(monash::Object* exp, monash::Environment* env);
monash::Object* evalSequence(monash::Objects* exps, monash::Environment* env);
bool isTrue(monash::Object* exp);


#endif // __SCHEME_H__
