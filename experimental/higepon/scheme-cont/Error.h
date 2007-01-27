#ifndef _ERROR_
#define _ERROR_

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <setjmp.h>
#include <string>
#include <stdio.h>
#include <stdarg.h>

namespace monash {

class Error
{
private:
    Error();
    virtual ~Error();

public:
    static void initialize();
    static void showErrorAndExit();
    static void raise(uint32_t lineno, std::string cppfile, uint32_t cpplineno, std::string cppfunc, const char* format, ...);

    static jmp_buf returnPoint;
    static std::string error;
    static std::string file;
    static std::string cppfile;
    static std::string cppfunc;
    static uint32_t lineno;
    static uint32_t cpplineno;
};

#define RAISE_ERROR(lineno, ...)                                     \
    Error::raise(lineno, __FILE__, __LINE__, __func__, __VA_ARGS__); \
    /* NOTREACHED */


}; // namespace monash

#endif // _ERROR_
