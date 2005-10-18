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
#else
#include <stdlib.h>
#include <stdio.h>
#endif

void
java::awt::FontMetrics::create (jbyteArray data)
{
	jbyte* data_p = elements(data);
#ifdef MONA
	monapi_cmemoryinfo* mi = monapi_call_file_decompress_st5_file("/MONA12.MF5", false);
	if (mi == NULL || mi->Size == 0) return;
	memcpy(data_p, mi->Data, mi->Size);
	monapi_cmemoryinfo_dispose(mi);
	monapi_cmemoryinfo_delete(mi);
#else
	FILE* fp;
	if((fp = fopen("./MONA12.MNF", "rb")) == NULL ) {
		printf("%s:%s:%d:ERROR: can not get font!\n", __FILE__, __FUNCTION__, __LINE__);
		exit(-1);
	}
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	if(fread(data_p, 1, size, fp) <= 0) {
		printf("%s:%s:%d:ERROR: can not get font!\n", __FILE__, __FUNCTION__, __LINE__);
		fclose(fp);
		exit(-1);
	} else {
		printf("%s:%s:%d: font loaded (%d bytes)\n", __FILE__, __FUNCTION__, __LINE__, (int)size);
		fclose(fp);
	}
#endif
}


