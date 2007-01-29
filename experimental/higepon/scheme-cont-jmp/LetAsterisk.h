#ifndef __LET_ASTERISK_H__
#define __LET_ASTERISK_H__

#include "Begin.h"
#include "Number.h"
#include "Lambda.h"
#include <vector>
#include <map>

namespace monash {

class LetAsterisk : public Object
{
public:
    LetAsterisk(Objects* body, Variables* variables, Objects* values, uint32_t lineno = 0);
    virtual ~LetAsterisk();

public:
    virtual std::string toString();
    virtual int type() const;
    virtual uint32_t lineno() const { return lineno_; }
    virtual Object* eval(Environment* env);
    virtual Object* expand();

protected:
    Objects* expandInternal(Variables::iterator variablesIt, Objects::iterator valuesIt);
    Objects* body_;
    Variables* variables_;
    Objects* values_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __LET_ASTERISK_H__
