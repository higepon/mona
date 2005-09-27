#ifndef __java_lang_Object__
#define __java_lang_Object__

#include <gcj/javaprims.h>

#pragma interface

#if __GNUC__ == 3 && __GNUC_MINOR__ == 3
class ::java::lang::Object
#else
class java::lang::Object
#endif
{
private:
  virtual void __pad_0 () { }
  virtual void __pad_1 () { }
public:  // actually protected
  virtual void finalize () { }
public:
  Object () { }
  ::java::lang::Class *getClass ();
  virtual jint hashCode ();
  virtual jboolean equals (::java::lang::Object *);
  virtual ::java::lang::String *toString ();

  static ::java::lang::Class class$;

#if __GNUC__ == 3 && __GNUC_MINOR__ == 3
private:
  jobject __pad_2;
#endif
};

#endif /* __java_lang_Object__ */
