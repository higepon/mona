// DO NOT EDIT THIS FILE - it is machine generated -*- c++ -*-

#ifndef __org_monaos_IO__
#define __org_monaos_IO__

#pragma interface

#include <java/lang/Object.h>

extern "Java"
{
  namespace org
  {
    namespace monaos
    {
      class IO;
    }
  }
}

class org::monaos::IO : public ::java::lang::Object
{
public:
  static jbyte inp8 (jint);
  static void outp8 (jint, jbyte);
  static jshort inp16 (jint);
  static void outp16 (jint, jshort);
  static jint inp32 (jint);
  static void outp32 (jint, jint);
  static void set_irq (jint, jboolean, jboolean);
  IO ();

  static ::java::lang::Class class$;
};

#endif /* __org_monaos_IO__ */
