/*!
    \file   Scanner.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
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

Scanner::Scanner(Reader* reader) : reader_(reader)
{

}

Scanner::~Scanner()
{

}

::util::String Scanner::getFileName()
{
    return reader_->getFileName();
}

int Scanner::getLineNo()
{
    return reader_->getLineNo();
}


//
// <token> => <identifier> | <boolean> | <number>
//          | <charcter> | <string> | <regexp>
//          | ( | ) | #( | ' | ` | , | ,@ | .
SToken* Scanner::getToken()
{
    char c = readChar();
    if (c == EOF) return NULL;
    SToken* token;
    while (isSpace(c))
    {
        c = readChar();
    }

    // <identifier> => <initial> <subsequent>*
    //               | <peculiar identifier>
    // <initial> => <letter> | <special initial>
    //
    // <letter> => a | b | c | ... | z
    //
    // <special initial> => ! | $ | % | & | * | / | : | < | =
    //                       | > | ? | ^ | _ | ~
    //                      We added '|' to special initial for a pipe.
    //
    // <subsequent> => <initial> | <digit> | <special subsequent>
    //
    // <special subsequent> => + | - | . | @
    //
    // <peculiar identifier> => + | - | ...
    if (isLetter(c) || isSpecialInitial(c) || c == '+' || c == '-' | c == '.')
    {
        String text;
        text += c;
        for (;;)
        {
            c = readChar();
            if (isDelimiter(c))
            {
                unReadChar(c);
                break;
            }
            text += c;
        }
        if ((text.startWith("+") || text.startWith("-")) && text.size() > 1)
        {
            for (int i = (int)text.size() - 1; i >= 0 ; i--)
            {
                unReadChar(text[i]);
            }
            goto other;
        }
        else if (text.startWith("...") && text.size() > 3)
        {
            for (int i = (int)text.size() - 1; i >= 0 ; i--)
            {
                unReadChar(text[i]);
            }
            goto other;
        }
        else if (text == ".")
        {
            unReadChar(text[0]);
            goto other;
        }

        if (isSynaticKeyword(text))
        {
            token = new SToken(SToken::KEYWORD);
            token->text = text;
            return token;
        }
        else
        {
            token = new SToken(SToken::VARIABLE);
            token->text = text;
            return token;
        }

    }
    else
    {
        unReadChar(c);
    }
other:
    c = readChar();
    switch(c)
    {
    case '(':
        return new SToken(SToken::LEFT_PAREN);
    case ')':
        return new SToken(SToken::RIGHT_PAREN);
    case '\'':
        return new SToken(SToken::SINGLE_QUOTE);
    case '.':
        return new SToken(SToken::PERIOD);
    default:
        break;
    }

    // <coomment> => ; <all subsequent characters up to a line break>
    if (c == ';')
    {
        String comment;
        for (;;)
        {
            c = readChar();
            if (c == '\n' || c == EOF) break;
            comment += c;
        }
        token = new SToken(SToken::COMMENT);
        token->text = comment;
        return token;
    }
    // , | ,@
    else if (c == ',')
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
        String text;
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
            for (;;)
            {
                c = readChar();
                if (isDelimiterWithoutCommentStart(c))
                {
                    unReadChar(c);
                    break;
                }
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
                SYNTAX_ERROR("invalide character #\\<%s>", text.data());
            }
            break;
        case '/':
            // <regexp> => #/ <any character except for '/' >/i*
            bool caseFold = false;
            for (;;)
            {
                c = readChar();
                if (c == '/')
                {
                    if (text.at(text.size() - 1) != '\\')
                    {
                        c = readChar();
                        if (c == 'i')
                        {
                            caseFold = true;
                            unReadChar(c);
                        }
                        break;
                    }
                }
                text += c;
            }
            token = new SToken(SToken::REGEXP);
            token->text = text;
            token->integer = caseFold ? 1 : 0;
            return token;
        }
    }
    // <string> => "<sring element>*"
    // <string element> => <any character other than " or backslash>
    //                   | backslash" | backslash
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
                else if (c == 'n')
                {
                    text += "\n";
                }
                else
                {
                    SYNTAX_ERROR("invalid character in string<%c>", c);
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
        String text;
        text += c;
        for (;;)
        {
            c = readChar();
            if (isDelimiter(c))
            {
                unReadChar(c);
                break;
            }
            else if (!isDigit(c))
            {
                SYNTAX_ERROR("invalid number");
            }
            text += c;
        }
        token = new SToken(SToken::NUMBER);
        token->integer = atoi(text.data());
        return token;
    }
    else if (c == EOF)
    {
        return NULL;
        printf("%s %s:%d[%c]\n", __func__, __FILE__, __LINE__, c);fflush(stdout);// debug
    }
    return NULL;
}

char Scanner::readChar()
{
    return reader_->readChar();
}

void Scanner::unReadChar(char c)
{
    reader_->unReadChar(c);
}

bool Scanner::isLetter(char c)
{
    return ('a' <= c && c <= 'z') || ('A' <= c && c<= 'Z');
}

bool Scanner::isSpecialInitial(char c)
{
    switch(c)
    {
    case '!':
    case '$':
    case '%':
    case '&':
    case '*':
    case '/':
    case ':':
    case '<':
    case '=':
    case '>':
    case '?':
    case '_':
    case '~':
    case '|':
        return true;
    default:
        return false;
    }
}

bool Scanner::isSpace(char c)
{
    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

bool Scanner::isDelimiter(char c)
{
    return c == ';' || isDelimiterWithoutCommentStart(c);
}

bool Scanner::isDelimiterWithoutCommentStart(char c)
{
    return isSpace(c) || c == '(' || c == ')' || c == '\"' || c == EOF;
}

bool Scanner::isDigit(char c)
{
    return '0' <= c && c <= '9';
}

bool Scanner::isExpressionKeyword(const String& s)
{
    return s == "quote" || s == "lambda" || s == "if"   || s == "set!"
        || s == "begin" || s == "cond"   || s == "and"  || s == "or"
        || s == "case"  || s == "let"    || s == "let*" || s == "letrec"
        || s == "do"    || s == "delay"  || s == "quasiquote";
}

bool Scanner::isSynaticKeyword(const String& s)
{
    return s == "else" || s == "define" || s == "unquote"
        || s == "unquote-splicing" || isExpressionKeyword(s);
}
