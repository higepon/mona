/*!
    \file  Parser.h
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
class Parser;
class Cons;
class Scanner;

class Parser
{
public:
    Parser(Scanner* scanner);
    virtual ~Parser();
    Object* parse();

protected:
    Object* parseDatum();
    Object* parseSimpleDatum();
    Object* parseCompoundDatum();
    Object* parseVector();
    Object* parseList();
    Token* nextToken();
    int tokenType();

    Scanner* scanner_;
    Token* token_;
};

}; // namespace monash

#endif // __EXT_RET_PARSER_H__
