// This file was created by `gcjh -stubs'. -*- c++ -*-
//
// This file is intended to give you a head start on implementing native
// methods using CNI.
// Be aware: running `gcjh -stubs ' once more for this class may
// overwrite any edits you have made to this file.

#include <gcj/javaprims.h>
#include <gcj/cni.h>
#include <java/lang/Object.h>
#include <java/lang/System.h>
#include <java/io/PrintStream.h>
#ifdef MONA
#include <monapi.h>
#else
#include <stdio.h>
#endif

::java::lang::Class *
java::lang::Object::getClass ()
{
	return **(jclass**)this;
}


jint
java::lang::Object::hashCode ()
{
	return (jint)this;
}


::java::lang::String *
java::lang::Object::toString ()
{
	return JvNewStringLatin1(
		((_Jv_Utf8Const*)*((void**)((jobject)getClass() + 1) + 1))->data);
}


