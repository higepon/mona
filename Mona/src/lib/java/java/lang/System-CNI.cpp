// This file was created by `gcjh -stubs'. -*- c++ -*-
//
// This file is intended to give you a head start on implementing native
// methods using CNI.
// Be aware: running `gcjh -stubs ' once more for this class may
// overwrite any edits you have made to this file.

#include <java/lang/System.h>
#include <gcj/cni.h>
#ifdef MONA
#include <monapi.h>
#else
#include <stdlib.h>
#endif

void
java::lang::System::exit (jint status)
{
	::exit(status);
}


