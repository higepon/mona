#include "Parser.h"
#include "Assert.h"
#include <stdio.h>

using namespace monash;

Parser::Parser(Tokenizer* tokenizer) : tokenizer_(tokenizer)
{
}

Parser::~Parser()
{
}

SExp* Parser::parse()
{
    SExp* sexp = NULL;
    Token token = tokenizer_->nextToken();
    switch(token.type)
    {
    case Token::LEFT_PAREN:
        sexp = new SExp(SExp::SEXPS);SCM_ASSERT(sexp);
        sexp->lineno = token.lineno;
        for (;;)
        {
//            printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
            SExp* child = parse();
            if (NULL == child) return sexp;
//            printf("<<%s>> \n", child->toString().data());
            sexp->sexps.add(child);
        }
    case Token::RIGHT_PAREN:

        return NULL;
    case Token::NUMBER:

        sexp = new SExp(SExp::NUMBER);SCM_ASSERT(sexp);
        sexp->value = token.value;
        sexp->lineno = token.lineno;
        return sexp;
    case Token::IDENTIFIER:

        if (token.text.startWith("#\\"))
        {
            sexp = new SExp(SExp::CHAR);SCM_ASSERT(sexp);
        }
        else
        {
            sexp = new SExp(SExp::SYMBOL);SCM_ASSERT(sexp);
        }
        sexp->text = token.text;
        sexp->lineno = token.lineno;
        return sexp;
    case Token::QUOTE:

        sexp = new SExp(SExp::QUOTE);SCM_ASSERT(sexp);
        sexp->text = token.text;
        sexp->lineno = token.lineno;
        return sexp;
    case Token::STRING:

        sexp = new SExp(SExp::STRING);SCM_ASSERT(sexp);
        sexp->text = token.text;
        sexp->lineno = token.lineno;
        return sexp;
    default:
        RAISE_ERROR(token.lineno, "unknown token");
    }

    return NULL;
}
