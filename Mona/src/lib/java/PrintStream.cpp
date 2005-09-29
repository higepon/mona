// This file was created by `gcjh -stubs'. -*- c++ -*-
//
// This file is intended to give you a head start on implementing native
// methods using CNI.
// Be aware: running `gcjh -stubs ' once more for this class may
// overwrite any edits you have made to this file.

#include <gcj/javaprims.h>
#include <java/io/PrintStream.h>
#include <gcj/cni.h>
#ifdef MONA
#include <monapi.h>
#else
#include <stdio.h>
#endif

void java::io::PrintStream::print(jint i) {
	printf("%d", (int)i);
}

void java::io::PrintStream::print(jfloat f) {
	printf("%f", (float)f);
}

void java::io::PrintStream::print(::java::lang::String* s) {
	int len = s->length();
	jchar* ch = _Jv_GetStringChars(s);
	for (int i = 0; i < len; i++, ch++) {
		printf("%c", (*ch < 128) ? (char)*ch : '?');
	}
}

void java::io::PrintStream::println() {
	printf("\n");
}
