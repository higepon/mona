#ifndef __QUOTE_H__
#define __QUOTE_H__

#include "Object.h"

namespace monash {

class Quote : public Object
{
public:
   Quote(const std::string& value);
   virtual ~Quote();

public:
   virtual std::string toString();
   virtual int type() const;
   virtual Object* eval(Environment* env);
   virtual std::string text() const { return value_;}

protected:
   std::string value_;
};

}; // namespace monash

#endif // __QUOTE_H__
