#ifndef __INPUT_PORT_H__
#define __INPUT_PORT_H__

#include "Object.h"
#include "Charcter.h"

namespace monash {

class InputPort : public Object
{
public:
    InputPort(FILE* stream, uint32_t lineno = 0);
    virtual ~InputPort();

public:
    virtual ::util::String toString();
    virtual int type() const;
    virtual uint32_t lineno() const { return lineno_; }
    virtual Object* eval(Environment* env);
    virtual bool eqv(Object* o);
    virtual bool eq(Object* o);
    virtual Charcter* readCharacter();
    virtual Charcter* peekCharacter();
    virtual void close();
    virtual bool charReady();

protected:
    FILE* stream_;
    uint32_t lineno_;

};

}; // namespace monash

#endif // __INPUT_PORT_H__
