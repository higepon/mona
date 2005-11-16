// This file was created by `gcjh -stubs'. -*- c++ -*-
//
// This file is intended to give you a head start on implementing native
// methods using CNI.
// Be aware: running `gcjh -stubs ' once more for this class may
// overwrite any edits you have made to this file.

#include <java/lang/Object.h>
#include <gcj/cni.h>
#ifdef MONA
#include <sms_gc/sms_gc.h>
#include <sms_gc/sms_ptr_dict.h>
#include <monapi.h>
#else
#include <sms_gc.h>
#include <sms_ptr_dict.h>
#include <string.h>
#endif

extern sms_ptr_dict_memory* sms_gc_get_info(void* addr);

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


jobject
java::lang::Object::clone ()
{
	sms_ptr_dict_memory* p = sms_gc_get_info(this);
	jobject copy = (jobject)sms_gc_malloc_type(p->size, p->type);
	memcpy(copy, this, p->size);
	return copy;
}


