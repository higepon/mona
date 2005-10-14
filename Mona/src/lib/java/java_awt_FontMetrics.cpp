// This file was created by `gcjh -stubs'. -*- c++ -*-
//
// This file is intended to give you a head start on implementing native
// methods using CNI.
// Be aware: running `gcjh -stubs ' once more for this class may
// overwrite any edits you have made to this file.

#include <java/awt/FontMetrics.h>
#include <gcj/cni.h>
#ifdef MONA
#include <monapi.h>
#include <monapi/messages.h>
#endif

void
java::awt::FontMetrics::create (jbyteArray data)
{
	//data = (jbyteArray)_Jv_NewPrimArray(&_Jv_byteClass, mi->Size);
	jbyte* data_p = elements(data);
#ifdef MONA
	monapi_cmemoryinfo* mi = monapi_call_file_decompress_st5_file("/MONA12.MF5", true);
	if (mi == NULL || mi->Size == 0) return;
	memcpy(data_p, mi->Data, mi->Size);
	monapi_cmemoryinfo_dispose(mi);
	monapi_cmemoryinfo_delete(mi);
#endif
}


