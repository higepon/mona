#include "Error.h"
#include "scheme.h"
using namespace monash;
using namespace std;

string Error::error;
string Error::file;
string Error::cppfile;
string Error::cppfunc;
jmp_buf Error::returnPoint;
Cont Error::cont;
uint32_t Error::lineno;
uint32_t Error::cpplineno;

Error::Error()
{
}

Error::~Error()
{
}

void Error::exitOnError()
{
    if (setjmp(returnPoint) != 0)
    {
        showErrorAndExit();
    }
}

void Error::returnOnError()
{
//begin:
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
//    if (setjmp(returnPoint) != 0)
    if (cont_save(&cont) != 0)
    {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        showError();
        //        goto begin;
    }

}

void Error::showErrorAndExit()
{
    SCHEME_WRITE(stderr, "%s:%d: error: %s\n", file.c_str(), lineno, error.c_str());
    SCHEME_WRITE(stderr, "%s:%d: debug: %s\n", cppfile.c_str(), cpplineno, cppfunc.c_str());
    exit(-1);
}

void Error::showError()
{
    SCHEME_WRITE(stderr, "error: %s\n", error.c_str());
    SCHEME_WRITE(stderr, "%s:%d: debug: %s\n", cppfile.c_str(), cpplineno, cppfunc.c_str());
}

void Error::raise(uint32_t l, string f, uint32_t cppl, string cppf, const char* format, ...)
{
    char buf[512];
    buf[0] = '\0';
    va_list args;
    int result;
    va_start(args, format);
    result = vsprintf(buf, format, args);
    va_end(args);
    if (result > 512)
    {
        /* over flow */
        SCHEME_WRITE(stderr, "error message over flow?\n");
    }

    error = buf;
    lineno = l;
    cppfile = f;
    cpplineno = cppl;
    cppfunc = cppf;
    longjmp(returnPoint, -1);
//    cont_restore(&cont, -1);
}
