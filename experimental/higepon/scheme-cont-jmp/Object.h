#ifndef __OBJECT_H__
#define __OBJECT_H__

#ifdef USE_BOEHM_GC
#define GC_NOT_DLL
#include "gc_cpp.h"
#endif
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Assert.h"
#include "Error.h"
#include <iostream>
#include <string>
#include "util/Vector.h"
#include "util/String.h"

namespace monash {

class Environment;
class Object;
typedef ::util::Vector<Object*> Objects;
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
    virtual ::util::String toString() { return "unknown object"; }
    virtual ::util::String toStringValue() { return toString(); }
    virtual bool eqv(Object* o) { return false; }
    virtual bool eq(Object* o) { return o == this; }
    virtual int type() const               = 0;
    virtual uint32_t lineno() const        = 0;
    virtual Object* eval(Environment* env) = 0;
    virtual Object* apply(Objects* arguments, Environment* env);
    bool isNumber() const { return type() == NUMBER; }
    bool isString() const { return type() == STRING; }
    bool isCharcter() const { return type() == CHARCTER; }
    bool isVector() const { return type () == VECTOR; }
    bool isQuote() const { return type() == QUOTE; }
    bool isPair() const { return type() == PAIR; }
    bool isTrue() const { return type() == TRUE; }
    bool isFalse() const { return type() == FALSE; }
    bool isBoolean() const { return isTrue() || isFalse(); }
    bool isUndef() const { return type() == UNDEF; }
    bool isCompoundProcedure() const { return type() == PROCEDURE; }
    bool isProcedure() const { return isCompoundProcedure() || isPrimitiveProcedure(); }
    bool isPrimitiveProcedure() const { return type() == PRIMITIVE_PROCEDURE; }
    bool isApplication() const { return type() == APPLICATION; }
    bool isBegin() const { return type() == BEGIN; }
    bool isLambda() const { return type() == LAMBDA; }
    bool isEnvironment() const { return type() == ENVIRONMENT; }
    bool isContinuation() const { return type() == CONTINUATION; }
    bool isOutputPort() const { return type() == OUTPUT_PORT; }
    bool isInputPort() const { return type() == INPUT_PORT; }
public:

    enum
    {
        NUMBER,
        STRING,
        CHARCTER,
        QUOTE,
        VECTOR,
        CONTINUATION,
        VARIABLE,
        ASSIGNMENT,
        IF,
        OUTPUT_PORT,
        INPUT_PORT,
        LAMBDA,
        PROCEDURE,
        ENVIRONMENT,
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
        NAMED_LET,
        TRUE,
        FALSE,
        UNDEF,
    };
};

}; // namespace monash

#endif // __OBJECT_H__
