#ifndef __OBJECT_H__
#define __OBJECT_H__

#define GC_NOT_DLL
#include "gc_cpp.h"
#include "Assert.h"
#include "Error.h"
#include <iostream>
#include <string>
#include <vector>

namespace monash {

class Environment;
class Object;
typedef std::vector<Object*> Objects;
#ifdef USE_BOEHM_GC
class Object : public gc_cleanup
//    class Object : public gc
#else
class Object
#endif
{
public:
    Object();
    virtual ~Object();

public:
    virtual std::string toString()         = 0;
    virtual std::string toStringValue() { return toString(); }
    virtual bool eqv(Object* o) { return false; }
    virtual bool eq(Object* o) { return o == this; }
    virtual int type() const               = 0;
    virtual uint32_t lineno() const        = 0;
    virtual Object* eval(Environment* env) = 0;
    bool isNumber() const { return type() == NUMBER; }
    bool isString() const { return type() == STRING; }
    bool isCharcter() const { return type() == CHARCTER; }
    bool isVector() const { return type () == VECTOR; }
    bool isQuote() const { return type() == QUOTE; }
    bool isPair() const { return type() == PAIR; }
    bool isTrue() const { return type() == TRUE; }
    bool isFalse() const { return type() == FALSE; }
    bool isCompoundProcedure() const { return type() == PROCEDURE; }
    bool isProcedure() const { return isCompoundProcedure() || isPrimitiveProcedure(); }
    bool isPrimitiveProcedure() const { return type() == PRIMITIVE_PROCEDURE; }
    bool isApplication() const { return type() == APPLICATION; }

public:

    enum
    {
        NUMBER,
        STRING,
        CHARCTER,
        QUOTE,
        VECTOR,
        VARIABLE,
        ASSIGNMENT,
        IF,
        LAMBDA,
        PROCEDURE,
        BEGIN,
        DEFINITION,
        COND,
        EVAL,
        PAIR,
        APPLICATION,
        PRIMITIVE_PROCEDURE,
        AND,
        OR,
        LET,
        LET_ASTERISK,
        TRUE,
        FALSE,
    };
};

}; // namespace monash

#endif // __OBJECT_H__
