/*!
    \file  Token.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __STOKEN_H__
#define __STOKEN_H__

#include "util/String.h"

namespace monash {

class Token
{
public:
    Token(int type);
    virtual ~Token();
    ::util::String valueString();
    ::util::String typeString();

    enum
    {
        COMMENT,
        LEFT_PAREN,
        RIGHT_PAREN,
        IDENTIFIER,
        KEYWORD,
        VARIABLE,
        BOOLEAN,
        NUMBER,
        CHARCTER,
        STRING,
        VECTOR_START,
        SINGLE_QUOTE,
        BACK_QUOTE,
        CAMMA,
        CAMMA_AT,
        PERIOD,
        REGEXP,
    };

    int type;
    int integer;
    ::util::String text;
};

}; // namespace monash
#endif // __STOKEN_H__
