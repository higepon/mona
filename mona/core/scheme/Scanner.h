/*!
    \file  Scanner.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __SCANNER_H__
#define __SCANNER_H__

#include "util/String.h"
#include "Token.h"
#include "Reader.h"
#include "StringReader.h"
#include "InputPort.h"

namespace monash {

class InputPort;
class StringReader;

class Scanner
{
public:
    Scanner(Reader* reader, StringReader* stringReader, InputPort* inputPort);
    virtual ~Scanner();

    Token* getToken();
    ::util::String getFileName();
    int getLineNo();
    void temp(const char* str);

protected:
    char readChar();
    void unReadChar(char c);
    bool isSpace(char c);
    bool isLetter(char c);
    bool isSpecialInitial(char c);
    bool isDelimiter(char c);
    bool isDelimiterWithoutCommentStart(char c);
    bool isDigit(char c);
    bool isExpressionKeyword(const ::util::String& s);
    bool isSynaticKeyword(const ::util::String& s);

    Reader* reader_;
    StringReader* stringReader_;
    InputPort* inputPort_;
};

}; // namespace monash
#endif // __SCANNER_H__
