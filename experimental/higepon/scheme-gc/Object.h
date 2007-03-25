#ifndef __OBJECT_H__
#define __OBJECT_H__

#ifdef USE_BOEHM_GC
#include "gc_cpp.h"
#include "gc_allocator.h"
#endif
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Assert.h"
#include "Error.h"
#include "util/Vector.h"
#include "util/String.h"

namespace monash {

class Environment;
class Object;
typedef ::util::Vector<Object*> Objects;
#ifdef USE_BOEHM_GC
//class Object : public gc_cleanup
class Object : public gc
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
    virtual int type() const                  = 0;
    virtual uint32_t lineno() const           = 0;
    virtual ::util::String typeString() const = 0;
    virtual Object* eval(Environment* env) = 0;
    virtual Object* apply(Objects* arguments, Environment* env);
    bool isRiteralConstant() const { return type() == RITERAL_CONSTANT; }
    bool isNumber() const { return type() == NUMBER; }
    bool isSString() const { return type() == STRING; }
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
    bool isVariable() const { return type() == VARIABLE; }
    bool isValues() const { return type() == VALUES; }
    bool isNil() const { return type() == NIL; }
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
        RITERAL_CONSTANT,
        AND,
        OR,
        LET,
        LET_ASTERISK,
        VALUES,
        NAMED_LET,
        TRUE,
        FALSE,
        UNDEF,
        NIL,
    };
};

}; // namespace monash

#endif // __OBJECT_H__
