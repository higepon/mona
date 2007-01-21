#ifndef __EVAL_H__
#define __EVAL_H__

#include "Object.h"
#include "scheme.h"

namespace monash {

class Translator;

class Quote;

class Eval : public Object
{
public:
    Eval(Translator& translator, Quote* quote, uint32_t lineno = 0);
    virtual ~Eval();

public:
    virtual std::string toString();
    virtual int type() const;
    virtual Object* eval(Environment* env);
    virtual uint32_t lineno() const { return lineno_; }
    virtual std::string text() const { return value_;}

protected:
    Translator& translator_;
    Quote* quote_;
    std::string value_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __EVAL_H__
