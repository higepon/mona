#ifndef __java_lang_Object__
#define __java_lang_Object__

#include <gcj/javaprims.h>

#pragma interface

class java::lang::Object
{
private:
  virtual void __pad_0 () { }
  virtual void __pad_1 () { }
public:  // actually protected
  virtual void finalize () { }
public:
  Object () { }

  static ::java::lang::Class class$;

private:
  jobject __pad_2;
};

#endif /* __java_lang_Object__ */
