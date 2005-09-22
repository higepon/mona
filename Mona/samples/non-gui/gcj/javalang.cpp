#include <monapi.h>

typedef int* IntPtr;

extern "C" void* _Jv_AllocObject(void* type, int size) {
	IntPtr vtable = ((IntPtr)type) + 13;
	for (;; vtable++) {
		if (*(vtable + 1) == 0 && *(vtable + 2) == 0 &&
			*(vtable + 3) == 0 && *(vtable + 4) == 0) {
			break;
		}
	}

	void* result = malloc(size);
	*(IntPtr)result = *vtable;
	IntPtr vt = (IntPtr)*vtable;
	return result;
}

extern "C" void* _Jv_AllocObjectNoFinalizer(void* type, int size) {
	return _Jv_AllocObject(type, size);
}

extern "C" void _Jv_InitClass() {
}

extern "C" void _Jv_RegisterClass() {
}

void* _Jv_intClass = 0;
