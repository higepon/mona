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

    Scanner* scanner_;

};

}; // namespace monash

#endif // __EXT_RET_PARSER_H__
