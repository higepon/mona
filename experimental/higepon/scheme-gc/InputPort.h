#ifndef __INPUT_PORT_H__
#define __INPUT_PORT_H__

#include "Object.h"
#include "Reader.h"
#include "Charcter.h"
#include "scheme.h"

namespace monash {

class InputPort : public Object , public Reader
{
public:
    InputPort(FILE* stream, uint32_t lineno = 0);
    virtual ~InputPort();

public:
    virtual ::util::String toString();
    virtual int type() const;
    virtual ::util::String typeString() const { return "input port"; }
    virtual uint32_t lineno() const { return lineno_; }
    virtual Object* eval(Environment* env);
    virtual bool eqv(Object* o);
    virtual bool eq(Object* o);
    virtual Object* readCharacter();
    virtual Object* peekCharacter();
    virtual void close();
    virtual bool charReady();
    virtual char readChar();
    virtual void unReadChar(char c);

protected:
    FILE* stream_;
    uint32_t lineno_;

};

}; // namespace monash

#endif // __INPUT_PORT_H__
