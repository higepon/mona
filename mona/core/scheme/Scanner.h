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
#include "SToken.h"
#include "Reader.h"

namespace monash {

// あとで消す
void scanner_test();

class Scanner
{
public:
    Scanner(Reader* reader);
    virtual ~Scanner();

    SToken* getToken();
    ::util::String getFileName();
    int getLineNo();

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
};

}; // namespace monash
#endif // __SCANNER_H__
