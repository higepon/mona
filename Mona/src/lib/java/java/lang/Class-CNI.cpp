// This file was created by `gcjh -stubs'. -*- c++ -*-
//
// This file is intended to give you a head start on implementing native
// methods using CNI.
// Be aware: running `gcjh -stubs ' once more for this class may
// overwrite any edits you have made to this file.

#include <java/lang/Class.h>
#include <gcj/cni.h>
#include <gcj/field.h>
#ifdef MONA
#include <sms_gc/sms_gc.h>
#include <sms_gc/sms_ptr_dict.h>
#include <monapi.h>
#else
#include <sms_gc.h>
#include <sms_ptr_dict.h>
#include <string.h>
#endif

extern sms_ptr_dict* _Jv_methodList;

void
java::lang::Class::initializeClass ()
{
	for (int i = 0; i < constants.size; i++) {
		if (constants.tags[i] == 8) {
			void** p = (void**)constants.data + i;
			sms_gc_register(p);
			*p = JvNewStringUTF(((_Jv_Utf8Const*)*p)->data);
		}
	}
	for (int i = 0; i < static_field_count; i++) {
		if (fields[i].type == NULL)
			fields[i].type = &::java::lang::String::class$;
		if (fields[i].isRef()) {
			*(void**)fields[i].u.addr = NULL;
			sms_gc_register(fields[i].u.addr);
		}
	}
	_Jv_methodList->add(this, 0, 0);
	for (int i = 0; i < method_count; i++) {
		_Jv_methodList->add(methods[i].ncode, (int)this, (int)&methods[i]);
		if (strcmp(methods[i].name->data, "<clinit>") == 0) {
			(*(void(*)())methods[i].ncode)();
		}
	}
	state = JV_STATE_DONE;
}


::java::lang::String *
java::lang::Class::getName ()
{
	return JvNewStringUTF(name->data);
}


