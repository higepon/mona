#ifndef __LET_H__
#define __LET_H__

#include "Begin.h"
#include "Number.h"
#include "Lambda.h"
#include <vector>
#include <map>

namespace monash {

class Application;

class Let : public Object
{
public:
    Let(Objects* body, Variables* variables, Objects* values, uint32_t lineno = 0);
    virtual ~Let();

public:
    virtual std::string toString();
    virtual int type() const;
    virtual uint32_t lineno() const { return lineno_; }
    virtual Object* eval(Environment* env);
    virtual Application* expand();

protected:
    Objects* body_;
    Variables* variables_;
    Objects* values_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __LET_H__
