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

protected:
    char readChar();
    void unReadChar(char c);
    bool isSpace(char c);
    bool isDelimiter(char c);
    bool isDigit(char c);
    Reader* reader_;
};

}; // namespace monash
#endif // __SCANNER_H__
