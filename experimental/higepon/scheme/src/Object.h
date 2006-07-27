#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <string>
#include <vector>

namespace monash {

class Environment;

class Object
{
public:
    Object();
    virtual ~Object();

public:
    virtual std::string toString()         = 0;
    virtual int type() const               = 0;
    virtual Object* eval(Environment* env) = 0;
    enum
    {
        NUMBER,
        STRING,
        QUOTE,
        VARIABLE,
        ASSIGNMENT,
        IF,
        LAMBDA,
        PROCEDURE,
        BEGIN,
        DEFINITION,
        COND,
        PAIR,
        APPLICATION,
        PRIMITIVE_PROCEDURE,
        AND,
        OR,
    };
};

typedef std::vector<Object*> Objects;

}; // namespace monash

#endif // __OBJECT_H__
