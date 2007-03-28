#ifndef __SCANNER_H__
#define __SCANNER_H__

#include "util/String.h"

namespace monash {

struct SToken
{
    SToken(int type) : type(type) {}
    virtual ~SToken() {}
    int type;
    ::util::String string;
    int integer;
    enum
    {
        IDENTIFIER,
        BOOLEAN,
        NUMBER,
        CHARCTER,
        STRING,
        PIPE,
        VECTOR_START,
        SINGLE_QUOTE,
        BACK_QUOTE,
        CAMMA,
        CAMMA_AT,
        PERIOD
    };
};

class Scanner
{
public:
    Scanner();
    virtual ~Scanner();

    SToken* getToken();

protected:
    bool isSpace(char c);
    bool isDelimiter(char c);

};

}; // namespace monash
#endif // __SCANNER_H__
