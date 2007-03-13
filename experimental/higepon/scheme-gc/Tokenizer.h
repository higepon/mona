#ifndef __TOKNIEZER_H__
#define __TOKNIEZER_H__

#ifdef USE_BOEHM_GC
#include "gc_cpp.h"
#include "gc_allocator.h"
#endif
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include "util/String.h"

namespace monash {

#ifdef USE_BOEHM_GC
struct Token : public gc_cleanup
//class SExp : public gc
#else
struct Token
#endif
//typedef struct Token
{
public:
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

#ifdef USE_BOEHM_GC
class Tokenizer : public gc_cleanup
//class SExp : public gc
#else
class Tokenizer
#endif
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
