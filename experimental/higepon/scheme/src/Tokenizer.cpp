#include "Tokenizer.h"

using namespace monash;

Tokenizer::Tokenizer(const std::string& input) : input_(input), postion_(0)
{
}

Tokenizer::~Tokenizer()
{
}

char Tokenizer::getChar()
{
    if (input_.size() <= postion_) return EOF;
    int c = input_[postion_];
    postion_++;
    return c;

}

void Tokenizer::unGetChar()
{
    postion_--;
    if (postion_ < 0) postion_ = 0;
}

Token Tokenizer::nextToken()
{
    int c;
    Token token;

once_more:
    c = getChar();
    switch(c)
    {
    case '(':
        token.type = Token::LEFT_PAREN;
        return token;
    case ')':
        token.type = Token::RIGHT_PAREN;
        return token;
    case '\'':
        token.type = Token::QUOTE;
        return token;
    }
    if (isspace(c)) goto once_more;
    if (c == ';')
    {
        for (;;)
        {
            c = getChar();
            if (c == '\n') goto once_more;
        }
    }
    if (isdigit(c))
    {
        int n = 0;
        do {
            n = n * 10 + c - '0';
            c = getChar();
        } while (isdigit(c));
        unGetChar();
        token.type = Token::NUMBER;
        token.value = n;
        return token;
    }
    if (c == '\"')
    {
        std::string str("");
        for (;;)
        {
            c = getChar();
            if (c == '\"') break;
            str += c;
        }
        token.text = str;
        token.type = Token::STRING;
        return token;
    }
    else
    {
        std::string str("");
        str += c;
        for (;;)
        {
            c = getChar();
            if (isspace(c) || c == '(' || c == ')' || c== '\'')
            {
                unGetChar();
                break;
            }
            str += c;
        }
        token.type = Token::IDENTIFIER;
        token.text = str;
        return token;
    }
}

