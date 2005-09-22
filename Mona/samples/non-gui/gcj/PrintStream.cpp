// This file was created by `gcjh -stubs'. -*- c++ -*-
//
// This file is intended to give you a head start on implementing native
// methods using CNI.
// Be aware: running `gcjh -stubs ' once more for this class may
// overwrite any edits you have made to this file.

#include <java/io/PrintStream.h>
#include <monapi.h>

void
java::io::PrintStream::print (jint i)
{
	printf("%d", i);
}


void
java::io::PrintStream::println ()
{
	printf("\n");
}


