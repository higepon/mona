#include "sms_gc/sms_gc_object.h"
#include "sms_gc/sms_gc.h"

extern "C" int* sms_gc_get_size_ptr(void*);

sms_gc_object::sms_gc_object() {
	sms_gc_add(this, sizeof(*this), SMS_GC_TYPE_CPP);
	_size_of = sms_gc_get_size_ptr(this);
}

sms_gc_object::~sms_gc_object() {
	sms_gc_remove(this);
}
