#include <sms_gc_object.h>
#include <sms_gc.h>
#include <sms_ptr_dict.h>

extern sms_ptr_dict_memory* sms_gc_get_info(void* addr);

sms_gc_object::sms_gc_object() {
	sms_gc_add(this, sizeof(*this), SMS_GC_TYPE_CPP);
	_size_of = &sms_gc_get_info(this)->size;
}

sms_gc_object::~sms_gc_object() {
	sms_gc_remove(this);
}
