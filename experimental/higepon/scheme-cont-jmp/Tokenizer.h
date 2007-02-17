#ifndef __TOKNIEZER_H__
#define __TOKNIEZER_H__

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <string>
#include <stdio.h>
#include "util/String.h"

namespace monash {

typedef struct Token
{
    int type;
    ::util::String text;
    int value;
    uint32_t lineno;
    enum
    {
        IDENTIFIER,
        STRING,
        NUMBER,
        LEFT_PAREN,
        RIGHT_PAREN,
        QUOTE,
    };
};

class Tokenizer
{
public:
    Tokenizer(const ::util::String& input);
    virtual ~Tokenizer();

    Token nextToken();

protected:
    char getChar();
    void unGetChar();

    ::util::String input_;
    uint32_t postion_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __TOKNIEZER_H__
