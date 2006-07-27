#ifndef __STRING_H__
#define __STRING_H__

#include "Object.h"

namespace monash {

class String : public Object
{
public:
    String(const std::string& value);
    virtual ~String();

public:
    virtual std::string toString();
    virtual int type() const;
    virtual Object* eval(Environment* env);
    virtual std::string value() { return value_; }

protected:
    std::string value_;
};

}; // namespace monash

#endif // __STRING_H__
