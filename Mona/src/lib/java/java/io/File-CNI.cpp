// This file was created by `gcjh -stubs'. -*- c++ -*-
//
// This file is intended to give you a head start on implementing native
// methods using CNI.
// Be aware: running `gcjh -stubs ' once more for this class may
// overwrite any edits you have made to this file.

#include <java/io/File.h>
#include <gcj/cni.h>
#ifdef MONA
#include <monapi.h>
#include <monapi/messages.h>

static char* get_string(jstring s) {
	int len = s->length();
	char* cstr = new char[len + 1];
	jchar* ss = JvGetStringChars(s);
	for (int i = 0; i < len; i++)
		cstr[i] = (char)ss[i];
	cstr[len] = '\0';
	return cstr;
}
#endif

JArray< ::java::lang::Object *> *
java::io::File::list (jstring pathname)
{
#ifdef MONA
	char* dir = get_string(pathname);
	monapi_cmemoryinfo* mi = monapi_call_file_read_directory(dir, MONAPI_TRUE);
	delete [] dir;

	int size = *(int*)mi->Data;
	if (mi == NULL || size == 0) {
		jobjectArray zero_array = _Jv_NewObjectArray(0, NULL, NULL);
		return zero_array;
	}

	jobjectArray array = _Jv_NewObjectArray(size, NULL, NULL);

	monapi_directoryinfo* p = (monapi_directoryinfo*)&mi->Data[sizeof(int)];
	for (int i = 0; i < size; i++, p++) {
		elements(array)[i] = _Jv_NewStringUTF(p->name);
	}

	monapi_cmemoryinfo_dispose(mi);
	monapi_cmemoryinfo_delete(mi);

	return array;
#else
	jobjectArray zero_array = _Jv_NewObjectArray(0, NULL, NULL);
	return zero_array;
#endif
}


