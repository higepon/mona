// This file was created by `gcjh -stubs'. -*- c++ -*-
//
// This file is intended to give you a head start on implementing native
// methods using CNI.
// Be aware: running `gcjh -stubs ' once more for this class may
// overwrite any edits you have made to this file.

#include <gcj/javaprims.h>
#include <java/lang/Object.h>
#include <gcj/cni.h>

::java::lang::String* java::lang::Object::toString() {
	return (jstring)*((void**)(**(jobject**)this + 1) + 1);
}
