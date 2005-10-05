#ifndef __SMS_GC_OBJECT_H__
#define __SMS_GC_OBJECT_H__

#define SMS_GC_CTOR() if (_size_of) *_size_of = sizeof(*this)
#define SMS_GC_CTOR2() if (_size_of) for (; *_size_of < (int)sizeof(*this); (*_size_of)++) ((char*)this)[*_size_of] = 0;

class sms_gc_object {
protected:
	int* _size_of;

public:
	sms_gc_object();
	virtual ~sms_gc_object();
};

#endif
