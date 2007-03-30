#ifndef __EXT_REP_PARSER_H__
#define __EXT_RET_PARSER_H__

#include "scheme.h"
#include "Scanner.h"

namespace monash {

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

    SToken* nextToken();
    int tokenType();

    Scanner* scanner_;
    SToken* token_;

};

}; // namespace monash

#endif // __EXT_RET_PARSER_H__
