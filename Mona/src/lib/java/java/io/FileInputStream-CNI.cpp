// This file was created by `gcjh -stubs'. -*- c++ -*-
//
// This file is intended to give you a head start on implementing native
// methods using CNI.
// Be aware: running `gcjh -stubs ' once more for this class may
// overwrite any edits you have made to this file.

#include <java/io/FileInputStream.h>
#include <gcj/cni.h>
#ifdef MONA
#include <monapi.h>
#include <monapi/messages.h>
#else
#include <stdlib.h>
#include <stdio.h>
#endif

static char* get_string(jstring s) {
	int len = s->length();
	char* cstr = new char[len + 1];
	jchar* ss = JvGetStringChars(s);
	for (int i = 0; i < len; i++)
		cstr[i] = (char)ss[i];
	cstr[len] = '\0';
	return cstr;
}

jint
java::io::FileInputStream::get_size (jstring name)
{
	char *filename = get_string(name);
#ifdef MONA
	monapi_cmemoryinfo* mi = NULL;
	char c = filename[strlen(filename) - 1];
	if (c == '5') {
		mi = monapi_call_file_decompress_st5_file(filename, false);
	} else if (c == '2') {
		mi = monapi_call_file_decompress_bz2_file(filename, false);
	} else {
		mi = monapi_call_file_read_data(filename, false);
	}
	if (mi == NULL || mi->Size == 0) return 0;
	int size = mi->Size;
	monapi_cmemoryinfo_dispose(mi);
	monapi_cmemoryinfo_delete(mi);
	return size;
#else
	FILE* fp;
	if((fp = fopen(filename, "rb")) == NULL ) {
		return false;
	}
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	fclose(fp);
	return size;
#endif
}


jboolean
java::io::FileInputStream::read_data (jstring name, jbyteArray data)
{
	jbyte* data_p = elements(data);
	char *filename = get_string(name);
#ifdef MONA
	monapi_cmemoryinfo* mi = NULL;
	char c = filename[strlen(filename) - 1];
	if (c == '5') {
		mi = monapi_call_file_decompress_st5_file(filename, false);
	} else if (c == '2') {
		mi = monapi_call_file_decompress_bz2_file(filename, false);
	} else {
		mi = monapi_call_file_read_data(filename, false);
	}
	if (mi == NULL || mi->Size == 0) return false;
	memcpy(data_p, mi->Data, mi->Size);
	monapi_cmemoryinfo_dispose(mi);
	monapi_cmemoryinfo_delete(mi);
	return true;
#else
	FILE* fp;
	if((fp = fopen(filename, "rb")) == NULL ) {
		return false;
	}
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	if(fread(data_p, 1, size, fp) <= 0) {
		fclose(fp);
		return false;
	} else {
		fclose(fp);
		return true;
	}
#endif
}


