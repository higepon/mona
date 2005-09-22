#include <java/lang/Object.h>
#include <sms_gc/sms_gc.h>
#ifdef MONA
#include <monapi.h>
#else
#include <stdio.h>
#include <string.h>
#endif

typedef int* IntPtr;

extern "C" jobject _Jv_AllocObject(jclass type, jint size) {
	void* result = sms_gc_malloc(size);
	//bzero(result, size);
	memset(result, 0, size);
	IntPtr vtable = (IntPtr)(((char*)type) + sizeof(::java::lang::Object) + 48);
	*(IntPtr)result = *vtable;
	IntPtr vt = (IntPtr)*vtable;
	return (jobject)result;
}

extern "C" jobject _Jv_AllocObjectNoFinalizer(jclass type, jint size) {
	return _Jv_AllocObject(type, size);
}

extern "C" void _Jv_InitClass() {
}

extern "C" void _Jv_RegisterClass() {
}

// dummy for types
extern "C" void _Jv_byteClass() {}
extern "C" void _Jv_charClass() {}
extern "C" void _Jv_shortClass() {}
extern "C" void _Jv_intClass() {}
extern "C" void _Jv_longClass() {}
extern "C" void _Jv_floatClass() {}
extern "C" void _Jv_doubleClass() {}

extern "C" void* _Jv_NewPrimArray(void* type, jint length) {
	int size = 4;
	if (type == _Jv_byteClass) {
		size = 1;
	} else if (type == _Jv_charClass || type == _Jv_shortClass) {
		size = 2;
	} else if (type == _Jv_longClass || type == _Jv_doubleClass) {
		size = 8;
	}
	int sz = sizeof(::java::lang::Object) + sizeof(jint) + (length + 1) * size;
	char* result = (char*)sms_gc_malloc(sz);
	//bzero(result, sz);
	memset(result, 0, sz);
	*(jint*)(result + sizeof(::java::lang::Object)) = length;
	return result;
}

extern "C" void _Jv_ThrowBadArrayIndex(int index) {
	printf("BadArrayIndexException: %d\n", index);
	*(int*)NULL = 0;
}
