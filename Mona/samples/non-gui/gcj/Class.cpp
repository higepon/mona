// This file was created by `gcjh -stubs'. -*- c++ -*-
//
// This file is intended to give you a head start on implementing native
// methods using CNI.
// Be aware: running `gcjh -stubs ' once more for this class may
// overwrite any edits you have made to this file.

#include <gcj/javaprims.h>
#include <gcj/cni.h>
#include <java/lang/Class.h>
#include <sms_gc.h>

void
java::lang::Class::initializeClass ()
{
	for (int i = 1; i < constants.size; i++) {
		if (constants.tags[i] == 8) {
			void** p = (void**)constants.data + i;
			sms_gc_register(p);
			*p = JvNewStringLatin1(((_Jv_Utf8Const*)*p)->data);
		}
	}
	state = JV_STATE_DONE;
}


