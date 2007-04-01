#ifndef _ERROR_
#define _ERROR_

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdarg.h>
extern "C" {
#include "libcont/cont.h"
};
#include "util/String.h"

namespace monash {

class Error
{
private:
    Error();
    virtual ~Error();

public:
    static void exitOnError();
    static void returnOnError();
    static void showErrorAndExit();
    static void showError();
    static void raise(uint32_t lineno, ::util::String cppfile, uint32_t cpplineno, ::util::String cppfunc, const char* format, ...);

    static jmp_buf returnPoint;
    static Cont cont;

    static ::util::String error;
    static ::util::String file;
    static ::util::String cppfile;
    static ::util::String cppfunc;
    static uint32_t lineno;
    static uint32_t cpplineno;
};

#define RAISE_ERROR(lineno, ...)                                     \
    Error::raise(lineno, __FILE__, __LINE__, __func__, __VA_ARGS__); \
    /* NOTREACHED */

#define RETURN_ON_ERROR(f)               \
    Error::file = f;                     \
                                         \
    if (setjmp(Error::returnPoint) != 0) \
    {                                    \
        Error::showError();              \
    }



}; // namespace monash

#endif // _ERROR_
