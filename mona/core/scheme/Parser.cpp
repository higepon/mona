/*!
    \file   Parser.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "scheme.h"

using namespace monash;

#define SYNTAX_ERROR(...) printf(__VA_ARGS__);printf(" at %s:%d\n", scanner_->getFileName().data(), scanner_->getLineNo());fflush(stdout);
#define NEW(symbol, value) new symbol(value, scanner_->getLineNo())
#define NEW2(symbol, v1, v2) new symbol(v1, v2, scanner_->getLineNo())

Parser::Parser(Scanner* scanner) : scanner_(scanner)
{
}

Parser::~Parser()
{
}

Object* Parser::parse()
{
    nextToken();
    Object* ret = parseDatum();
    return ret;
}

Token* Parser::nextToken()
{
    return (token_ = scanner_->getToken());
}

// private

// <datum> => <simple datum> | <compound datum>
// <simple datum> => <boolean> | <number> | <character> | <string> | <regexp>
//                 | <symbol>
Object* Parser::parseDatum()
{
    if (token_ == NULL)
    {
        return SCM_EOF;
    }
    else if (token_->type == Token::COMMENT)
    {
        for (;;)
        {
            nextToken();
            if (token_ == NULL) return SCM_EOF;
            if (token_->type != Token::COMMENT) break;
        }
    }

    switch(token_->type)
    {
    case Token::BOOLEAN:
    case Token::NUMBER:
    case Token::CHARCTER:
    case Token::STRING:
    case Token::VARIABLE:
    case Token::KEYWORD:
    case Token::REGEXP:
        return parseSimpleDatum();
    default:
        return parseCompoundDatum();
    }
}

// <simple datum> => <boolean> | <number> | <character> | <string> | <regexp>
//                 | <symbol>
Object* Parser::parseSimpleDatum()
{
    switch(token_->type)
    {
    case Token::BOOLEAN:
        if (token_->integer == 1)
        {
            return SCM_TRUE;
        }
        else
        {
            return SCM_FALSE;
        }
    case Token::NUMBER:
        return NEW(Number, token_->integer);
        break;
    case Token::CHARCTER:
        return NEW(Charcter, "#\\" + token_->text);
    case Token::STRING:
        return NEW(SString, token_->text);
    case Token::VARIABLE:
    case Token::KEYWORD:
        return NEW(Identifier, token_->text);
    case Token::REGEXP:
        return new SRegexp(token_->text, token_->integer == 1 ? true : false, scanner_->getLineNo());
    default:
        SYNTAX_ERROR("invalid simple datum %s:%s\n", token_->typeString().data(), token_->valueString().data());
        break;
    }
    return SCM_EOF;
}

// <compound datum> => <list> | <vector>
// <list> => (<datum>*) | (<datum>+ . <datum>) | <abbreviation>
// <abbreviation> => <abbrev prefix> <datum>
// <abbrev prefix> => ' | ` | , | ,@
// <vector> => #(<datum>*)
Object* Parser::parseCompoundDatum()
{
    switch(token_->type)
    {
    case Token::VECTOR_START:
        return parseVector();
    case Token::LEFT_PAREN:
    case Token::SINGLE_QUOTE:
    case Token::BACK_QUOTE:
    case Token::CAMMA:
    case Token::CAMMA_AT:
        return parseList();
    default:
        SYNTAX_ERROR("list or vector should be here, but got %s %s", token_->typeString().data(), token_->valueString().data());
    }
    return SCM_EOF;
}

// <vector> => #(<datum>*)
Object* Parser::parseVector()
{
    if (token_->type != Token::VECTOR_START)
    {
        SYNTAX_ERROR("vector expected, but got %s %s", token_->typeString().data(), token_->valueString().data());
        return SCM_EOF;
    }
    Objects* objects = new Objects;
    for (;;)
    {
        nextToken();
        if (token_->type == Token::RIGHT_PAREN)
        {
            break;
        }
        objects->add(parseDatum());
    }
    Vector* v = NEW(Vector, objects->size());
    for (int i = 0; i < objects->size(); i++)
    {
        v->set(i, objects->get(i));
    }
    return v;
}

// <list> => (<datum>*) | (<datum>+ . <datum>) | <abbreviation>
// <abbreviation> => <abbrev prefix> <datum>
// <abbrev prefix> => ' | ` | , | ,@
Object* Parser::parseList()
{
    // (<datum>*) | (<datum>+ . <datum>)
    if (token_->type == Token::LEFT_PAREN)
    {
        Objects* objects = new Objects;
        for (;;)
        {
            nextToken();
            if (token_->type == Token::COMMENT)
            {
                for (;;)
                {
                    nextToken();
                    if (token_ == NULL) return SCM_EOF;
                    if (token_->type != Token::COMMENT) break;
                }
            }

            if (token_->type == Token::PERIOD)
            {
                nextToken();
                Object* o = parseDatum();
                nextToken();
                if (token_->type != Token::RIGHT_PAREN)
                {
                    SYNTAX_ERROR("invalid list, . position is wrong");
                    return NULL;
                }
                if (objects->size() == 0)
                {
                    SYNTAX_ERROR("invalid list, . position is wrong");
                    return NULL;
                }

                Cons* ret;
                // (objects[0], objects[1] ... . o)
                SCM_LIST_CONS(objects, o, ret, scanner_->getLineNo());
                return ret;
            }
            else if (token_->type == Token::RIGHT_PAREN)
            {
                if (objects->size() == 0) return SCM_NIL;
                Cons* ret;
                SCM_LIST(objects, ret, scanner_->getLineNo());
                return ret;
            }
            else
            {
                objects->add(parseDatum());
            }
        }
    }
    else
    {
        // <abbreviation> => <abbrev prefix> <datum>
        // <abbrev prefix> => ' | ` | , | ,@
        switch(token_->type)
        {
        case Token::SINGLE_QUOTE:
            nextToken();
            return NEW2(Cons, new Identifier("quote"), new Cons(parseDatum(), SCM_NIL));
        case Token::BACK_QUOTE:
            nextToken();
            return NEW2(Cons, new Identifier("quasiquote"), new Cons(parseDatum(), SCM_NIL));
        case Token::CAMMA_AT:
            nextToken();
            return NEW2(Cons, new Identifier("unquote-splicing"), new Cons(parseDatum(), SCM_NIL));
        case Token::CAMMA:
            nextToken();
            return NEW2(Cons, new Identifier("unquote"), new Cons(parseDatum(), SCM_NIL));
        default:
            SYNTAX_ERROR("soory not supported\n");
            return NULL;
        }
    }
    return NULL;
}
