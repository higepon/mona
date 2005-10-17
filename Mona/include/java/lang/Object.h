#ifndef __JAVA_LANG_OBJECT_H__
#define __JAVA_LANG_OBJECT_H__

#include <gcj/javaprims.h>

#pragma interface

class java::lang::Object
{
private:
  virtual void nacd_1 () { }
  virtual void nacd_2 () { }
public:  // actually protected
  virtual void finalize () { }
public:
  Object () { }
  jclass getClass ();
  virtual jint hashCode ();
  virtual jboolean equals (jobject);
  virtual jstring toString ();
protected:
  virtual jobject clone ();

  static ::java::lang::Class class$;

#ifndef JV_HASH_SYNCHRONIZATION
private:
  jobject sync_info;
#endif
};

#endif /* __JAVA_LANG_OBJECT_H__ */
