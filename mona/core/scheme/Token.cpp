/*!
    \file   Token.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "Token.h"
#include "scheme.h"

using namespace util;
using namespace monash;

Token::Token(int type) : type(type)
{
}

Token::~Token()
{
}

String Token::typeString()
{
    switch(type)
    {
    case COMMENT:
        return "COMMENT";
    case LEFT_PAREN:
        return "LEFT_PAREN";
    case RIGHT_PAREN:
        return "RIGHT_PAREN";
    case IDENTIFIER:
        return "IDENTIFIER";
    case KEYWORD:
        return "KEYWORD";
    case VARIABLE:
        return "VARIABLE";
    case CHARCTER:
        return "CHARCTER";
    case STRING:
        return "STRING";
    case VECTOR_START:
        return "VECTOR_START";
    case SINGLE_QUOTE:
        return "SINGLE_QUOTE";
    case BACK_QUOTE:
        return "BACK_QUOTE";
    case CAMMA:
        return "CAMMA";
    case CAMMA_AT:
        return "CAMMA_AT";
    case PERIOD:
        return "PERIOD";
    case BOOLEAN:
        return "BOOLEAN";
    case NUMBER:
        return "NUMBER";
    default:
        fprintf(stderr, "unknown token[%d]\n", type);
        return "";
    };
}

String Token::valueString()
{
    String ret;
    char buf[32];
    switch(type)
    {
    case COMMENT:
    case IDENTIFIER:
    case VARIABLE:
    case CHARCTER:
    case KEYWORD:
        return text;
    case REGEXP:
        return text;
    case STRING:
        ret = "\"";
        ret += text;
        ret += "\"";
        return ret;
    case RIGHT_PAREN:
        return ")";
    case LEFT_PAREN:
        return "(";
    case VECTOR_START:
        return "#(";
    case SINGLE_QUOTE:
        return "\'";
    case BACK_QUOTE:
        return "`";
    case CAMMA:
        return ",";
    case CAMMA_AT:
        return ",@";
    case PERIOD:
        return ".";
    case BOOLEAN:
        return integer == 1 ? "#t" : "#f";
    case NUMBER:
        sprintf(buf, "%d", integer);
        return buf;
    default:
        fprintf(stderr, "unknown token[%d]\n", type);
        return "";
    };
}
