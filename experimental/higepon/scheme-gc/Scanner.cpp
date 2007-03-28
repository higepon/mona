#include "Scanner.h"

using namespace util;
using namespace monash;

#define SYNTAX_ERROR //

char readChar()
{
    return ' ';
}

Scanner::Scanner()
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
    if (c == '#')
    {
        c = readChar();
        switch(c)
        {
        // boolean => #t | #f
        case 't':
            token = new SToken(SToken::BOOLEAN);
            token->integer = 1;
            break;
        case 'f':
            token = new SToken(SToken::BOOLEAN);
            token->integer = 0;
            break;
        case '(':
            token = new SToken(SToken::VECTOR_START);
            break;
        case '\\':
            // character => #\ <any character>
            //            | #\ <character name>
            // character name => space | newline
            String text;
            for (;;)
            {
                c = readChar();
                if (isDelimiter(c)) break;
                text += c;
            }

            break;
        }

    }
}

bool Scanner::isSpace(char c)
{
    return c == ' ' || c == '\n' || c == '\r';
}

bool Scanner::isDelimiter(char c)
{
    return isSpace(c) || c == '|' || '\"' || ';';
}
