#include <java/lang/System.h>
#include <java/io/PrintStream.h>
#include <gcj/cni.h>
#include <sms_gc/sms_gc.h>
#include <sms_gc/sms_ptr_dict.h>
#ifdef MONA
#include <monapi.h>
#else
#include <stdio.h>
#include <string.h>
#endif

sms_ptr_dict* _Jv_methodList = NULL;

::java::lang::Class _Jv_voidClass, _Jv_booleanClass, _Jv_byteClass, _Jv_charClass, _Jv_shortClass, _Jv_intClass, _Jv_longClass, _Jv_floatClass, _Jv_doubleClass;

class java::lang::VMClassLoader : public ::java::lang::Object {
public:
	static void initialize() {
		_Jv_voidClass   .vtable = JV_PRIMITIVE_VTABLE;
		_Jv_booleanClass.vtable = JV_PRIMITIVE_VTABLE;
		_Jv_byteClass   .vtable = JV_PRIMITIVE_VTABLE;
		_Jv_charClass   .vtable = JV_PRIMITIVE_VTABLE;
		_Jv_shortClass  .vtable = JV_PRIMITIVE_VTABLE;
		_Jv_intClass    .vtable = JV_PRIMITIVE_VTABLE;
		_Jv_longClass   .vtable = JV_PRIMITIVE_VTABLE;
		_Jv_floatClass  .vtable = JV_PRIMITIVE_VTABLE;
		_Jv_doubleClass .vtable = JV_PRIMITIVE_VTABLE;
	}
};

extern "C" sms_ptr_dict* sms_gc_new_ptr_dict();

jint _Jv_CreateJavaVM(void* vm_args) {
	_Jv_methodList = sms_gc_new_ptr_dict();
	::java::lang::VMClassLoader::initialize();
	JvInitClass(&::java::lang::System::class$);
	::java::lang::System::out = new ::java::io::PrintStream(NULL);
	return 0;
}

extern "C" jobject _Jv_AllocObject(jclass type, jint size) {
	JvInitClass(type);
	void* result = sms_gc_malloc(size);
	memset(result, 0, size);
	*(void**)result = type->vtable;
	if (type == &::java::lang::String::class$)
		((jstring)result)->boffset = sizeof(::java::lang::Object) + sizeof(jsize);
	return (jobject)result;
}

extern "C" jobject _Jv_AllocObjectNoFinalizer(jclass type, jint size) {
	if (type == NULL) {
		type = &::java::lang::String::class$;
		size = sizeof(::java::lang::String);
	}
	return _Jv_AllocObject(type, size);
}

extern "C" void _Jv_RegisterClass(jclass* classes) {
}

extern "C" jobject _Jv_NewPrimArray(jclass type, jsize length) {
	int size = 4;
	if (type == &_Jv_byteClass || type == &_Jv_booleanClass) {
		size = 1;
	} else if (type == &_Jv_charClass || type == &_Jv_shortClass) {
		size = 2;
	} else if (type == &_Jv_longClass || type == &_Jv_doubleClass) {
		size = 8;
	}
	int sz = sizeof(::java::lang::Object) + sizeof(jsize) + (length + 1) * size;
	jobject result = (jobject)sms_gc_malloc_type(sz, SMS_GC_TYPE_IGNORE);
	memset(result, 0, sz);
	*(jsize*)(result + 1) = length;
	return result;
}

extern "C" jobjectArray _Jv_NewObjectArray(jsize length, jclass klass, jobject init) {
	int sz = sizeof(::java::lang::Object) + sizeof(jsize) + (length + 1) * sizeof(jobject);
	jobjectArray result = (jobjectArray)sms_gc_malloc(sz);
	if (init == NULL) {
		memset(result, 0, sz);
	} else {
		jobject* p = elements(result);
		for (int i = 0; i < length; i++)
			p[i] = init;
	}
	*(jsize*)((jobject)result + 1) = length;
	return result;
}

extern "C" void _Jv_CheckArrayStore(jobjectArray array, jobject obj) {
}

extern "C" jobject _Jv_CheckCast(jclass klass, jobject obj) {
	return obj;
}


// String

jstring _Jv_NewStringLatin1(const char* bytes, jsize len) {
	jstring s = new ::java::lang::String();
	s->data = JvNewCharArray(len);
	s->count = len;
	jchar* ch = _Jv_GetStringChars(s);
	for (int i = 0; i < len; i++) ch[i] = (unsigned char)bytes[i];
	return s;
}

jstring _Jv_NewStringUTF(const char* bytes) {
	int len = 0;
	for (const unsigned char* p = (const unsigned char*)bytes; *p != '\0'; p++, len++) {
		if (*p >= 0xe0)
			p += 2;
		else if (*p >= 0xc0)
			p++;
	}
	jstring s = new ::java::lang::String();
	s->data = JvNewCharArray(len);
	s->count = len;
	jchar* ch = _Jv_GetStringChars(s);
	for (const unsigned char* p = (const unsigned char*)bytes; *p != '\0'; p++, ch++) {
		if (*p >= 0xe0) {
			*ch = ((p[0] & 0x0f) << 12) | ((p[1] & 0x3f) << 6) | (p[2] & 0x3f);
			p += 2;
		} else if (*p >= 0xc0) {
			*ch = ((p[0] & 0x1f) << 6) | (p[1] & 0x3f);
			p++;
		} else {
			*ch = *p & 0x7f;
		}
	}
	return s;
}


// Exceptions

extern "C" void* sms_gc_get_end_stack();

extern "C" void _Jv_PrintStackTrace() {
	char* start_stack;
	asm("movl %%ebp, %0" : "=g"(start_stack));
	char* end_stack = (char*)sms_gc_get_end_stack() - sizeof(void*);
	for (char* p = start_stack; p <= end_stack; p++) {
		void* addr = *(void**)p;
		sms_ptr_dict_memory* data = NULL;
		sms_ptr_dict_linear* pp = _Jv_methodList->get_first();
		for (; pp != NULL; pp = _Jv_methodList->get_next()) {
			for (int i = 0; i < pp->get_length(); i++) {
				sms_ptr_dict_memory* m = pp->get_data(i);
				if (m->addr > addr) break;
				data = m;
			}
		}
		if (data != NULL && data->size != 0) {
			jclass klass = (jclass)data->size;
			_Jv_Method* method = (_Jv_Method*)data->type;
			if (method->name->data[0] != '<') {
				printf("    at ");
				::java::lang::System::out->print(klass->getName());
				printf(".%s\n", method->name->data);
			}
		}
	}
}

extern "C" void _Jv_ThrowNullPointerException() {
	printf("NullPointerException\n");
	_Jv_PrintStackTrace();
	*(int*)NULL = 0;
}

extern "C" void _Jv_ThrowBadArrayIndex(int index) {
	printf("ArrayIndexOutOfBoundsException: %d\n", index);
	_Jv_PrintStackTrace();
	*(int*)NULL = 0;
}
