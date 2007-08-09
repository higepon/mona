/*!
    \file  ExtRepParser.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __EXT_REP_PARSER_H__
#define __EXT_RET_PARSER_H__

#include "scheme.h"
#include "Scanner.h"

namespace monash {
class ExtRepParser;
class Pair;
// External representations Parser
class ExtRepParser
{
public:
    ExtRepParser(Scanner* scanner);
    virtual ~ExtRepParser();
    Object* parse();

protected:
    Object* parseDatum();
    Object* parseSimpleDatum();
    Object* parseCompoundDatum();
    Object* parseVector();
    Object* parseList();

    void findSelfCall(Object* o);
    void findNameCall(Pair* p, const ::util::String& name);

    SToken* nextToken();
    int tokenType();

    Scanner* scanner_;
    SToken* token_;
};

}; // namespace monash

#endif // __EXT_RET_PARSER_H__
