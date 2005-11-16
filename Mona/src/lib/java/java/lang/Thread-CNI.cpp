// This file was created by `gcjh -stubs'. -*- c++ -*-
//
// This file is intended to give you a head start on implementing native
// methods using CNI.
// Be aware: running `gcjh -stubs ' once more for this class may
// overwrite any edits you have made to this file.

#include <java/lang/Thread.h>
#include <gcj/cni.h>
#ifdef MONA
#include <monapi.h>
#endif

void
Thread::sleep (jlong millis)
{
#ifdef MONA
	int ms = (int)millis;
	if (ms < 10) ms = 10;
	syscall_sleep(ms / 10);
#endif
}


