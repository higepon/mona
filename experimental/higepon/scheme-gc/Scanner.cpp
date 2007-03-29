#include "Scanner.h"
#include "scheme.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include "StringReader.h"

using namespace util;
using namespace monash;

#define SYNTAX_ERROR(...) printf(__VA_ARGS__);fflush(stdout);

// const char* input = "#t #\\a #\\b #\\space";
// static int pos = 0;
// // char readChar()
// // {
// //     if (pos >= strlen(input)) return -1;
// //     char c = input[pos];
// //     pos++;
// //     return c;
// // }

// void scanner_test()
// {
//     Reader* reader = new StringReader(input);
//     Scanner scanner(reader);
//     Strings expected;
//     expected.add(new String("BOOLEAN"));
//     expected.add(new String("#t"));
//     expected.add(new String("CHARCTER"));
//     expected.add(new String("a"));

//     for (int i = 0; i < expected.size() / 2; i++)
//     {
//         SToken* token = scanner.getToken();
//         String* type = expected[i * 2];
//         String* value = expected[i * 2 + 1];
//         if (token->typeString() == type->data() && token->valueString() == value->data())
//         {
//             printf("OK\n");
//         }
//         else
//         {
//             printf("NG\n");
//             printf("expected <%s %s> : but got <%s %s>\n", type->data(), value->data(), token->typeString().data(), token->valueString().data());
//         }
        
//     }

// }

Scanner::Scanner(Reader* reader) : reader_(reader)
{

}

Scanner::~Scanner()
{

}

//
// <token> => <identifier> | <boolean> | <number>
//          | <charcter> | <string>
//          | (|) | #( | ' | ` | , | ,@ | .
SToken* Scanner::getToken()
{
    char c = readChar();
    SToken* token;
    while (isSpace(c))
    {
        c = readChar();
    }

    switch(c)
    {
    case '|':
        return new SToken(SToken::PIPE);
    case '\'':
        return new SToken(SToken::SINGLE_QUOTE);
    case '.':
        return new SToken(SToken::PERIOD);
    defautl:
        break;
    }

    // , | ,@
    if (c == ',')
    {
        c =readChar();
        if (c == '@')
        {
            token = new SToken(SToken::CAMMA_AT);
            return token;
        }
        else
        {
            unReadChar(c);
            token = new SToken(SToken::CAMMA);
            return token;
        }
    }
    else if (c == '#')
    {
        c = readChar();
        switch(c)
        {
        // <boolean> => #t | #f
        case 't':
            token = new SToken(SToken::BOOLEAN);
            token->integer = 1;
            return token;
        case 'f':
            token = new SToken(SToken::BOOLEAN);
            token->integer = 0;
            return token;
        case '(':
            token = new SToken(SToken::VECTOR_START);
            return token;
        case '\\':
            // <character> => #\ <any character>
            //              | #\ <character name>
            // <character name> => space | newline
            String text;
            for (;;)
            {
                c = readChar();
                if (isDelimiter(c)) break;
                text += c;
            }

            if (text.size() == 0)
            {
                SYNTAX_ERROR("invalide character #\\");
            }
            else if (text.size() == 1 || text == "space" || text == "newline")
            {
                token = new SToken(SToken::CHARCTER);
                token->text = text;
                return token;
            }
            else
            {
                SYNTAX_ERROR("invalide character #\\%s", text.data());
            }
            break;
        }
    }
    // <string> => "<sring element>*"
    // <string element> => <any character other than " or \>
    //                   | \" | \\
    else if (c == '\"')
    {
        String text;
        for (;;)
        {
            c = readChar();
            if (c == '\\')
            {
                c = readChar();
                if (c == '\\')
                {
                    text += "\\";
                }
                else if (c == '\"')
                {
                    text += "\"";
                }
                else
                {
                    SYNTAX_ERROR("invalid character in string");
                }
            }
            else if (c == '\"')
            {
                break;
            }
            else
            {
                text += c;
            }
        }
        token = new SToken(SToken::STRING);
        token->text = text;
        return token;
    }
    // <number> => <num 10>
    // <num 10> =>  <complex 10>
    else if (c == '+' || c == '-' || isDigit(c))
    {
// mandokuse
        if (c == '+') 1;
    }
}

char Scanner::readChar()
{
    return reader_->readChar();
}

void Scanner::unReadChar(char c)
{
    reader_->unReadChar(c);
}


bool Scanner::isSpace(char c)
{
    return c == ' ' || c == '\n' || c == '\r';
}

bool Scanner::isDelimiter(char c)
{
    return isSpace(c) || c == '|' || c == '\"' || c == ';' || c == EOF;
}

bool Scanner::isDigit(char c)
{
    return '0' <= c && c <= '9';
}
