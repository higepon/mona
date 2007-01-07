#ifndef __SPECIALIF_H__
#define __SPECIALIF_H__

#include "scheme.h"

namespace monash {

class SpecialIf : public Object
{
public:
    SpecialIf(Object* predicate, Object* consequent, Object* alternative, uint32_t lineno = 0);
    virtual ~SpecialIf();

public:
    virtual std::string toString();
    virtual int type() const;
    virtual Object* eval(Environment* env);
    virtual uint32_t lineno() const { return lineno_; }
    Object* predicate() const { return predicate_; }
    Object* consequent() const { return consequent_; }
    Object* alternative() const { return alternative_; }

protected:
    Object* predicate_;
    Object* consequent_;
    Object* alternative_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __SPECIALIF_H__
