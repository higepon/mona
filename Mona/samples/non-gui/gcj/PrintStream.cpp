// This file was created by `gcjh -stubs'. -*- c++ -*-
//
// This file is intended to give you a head start on implementing native
// methods using CNI.
// Be aware: running `gcjh -stubs ' once more for this class may
// overwrite any edits you have made to this file.

#include <gcj/javaprims.h>
#include <java/io/PrintStream.h>
#ifdef MONA
#include <monapi.h>
#else
#include <stdio.h>
#endif

void java::io::PrintStream::print(jint i) {
	printf("%d", i);
}

void java::io::PrintStream::print(::java::lang::String* s) {
	printf("%s", ((_Jv_Utf8Const*)s)->data);
}

void java::io::PrintStream::println() {
	printf("\n");
}
