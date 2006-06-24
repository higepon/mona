#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <string>
#include <vector>

namespace monash {

class Object
{
public:
    Object();
    virtual ~Object();

public:
    virtual std::string toString() = 0;
    virtual int type() const       = 0;
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
    };
};

typedef std::vector<Object*> Objects;

}; // namespace monash

#endif // __OBJECT_H__
