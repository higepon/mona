#ifndef __TOKNIEZER_H__
#define __TOKNIEZER_H__

#include <string>

namespace monash {

typedef struct Token
{
    int type;
    std::string text;
    int value;
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
    Tokenizer(const std::string& input);
    virtual ~Tokenizer();

    Token nextToken();

protected:
    char getChar();
    void unGetChar();

    std::string input_;
    int postion_;
};

}; // namespace monash

#endif // __TOKNIEZER_H__
