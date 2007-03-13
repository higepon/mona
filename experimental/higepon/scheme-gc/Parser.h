#ifndef __PARSER_H__
#define __PARSER_H__

#include "Tokenizer.h"
#include "SExp.h"

namespace monash {

class SExp;

#ifdef USE_BOEHM_GC
class Parser : public gc_cleanup
//class SExp : public gc
#else
class Parser
#endif
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
