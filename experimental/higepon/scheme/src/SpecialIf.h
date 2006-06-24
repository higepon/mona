#ifndef __SPECIALIF_H__
#define __SPECIALIF_H__

#include "scheme.h"

namespace monash {

class SpecialIf : public Object
{
public:
    SpecialIf(Object* predicate, Object* consequent, Object* alternative);
    virtual ~SpecialIf();

public:
    virtual std::string toString();
    virtual int type() const;
    Object* predicate() const { return predicate_; }
    Object* consequent() const { return consequent_; }
    Object* alternative() const { return alternative_; }
    static Object* eval(SpecialIf* specialIf, Environment* env);

protected:
    Object* predicate_;
    Object* consequent_;
    Object* alternative_;
};

}; // namespace monash

#endif // __SPECIALIF_H__
