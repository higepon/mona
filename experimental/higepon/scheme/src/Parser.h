#ifndef __PARSER_H__
#define __PARSER_H__

#include "Tokenizer.h"
#include "Node.h"

namespace monash {

class SExp;

class Parser
{
public:
    Parser(Tokenizer* tokenizer);
    virtual ~Parser();
    SExp* parse();

protected:
    Tokenizer* tokenizer_;
};

}; // namespace monash

#endif // __PARSER_H__
