// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#include <monapi.h>
#include <monapi/messages.h>

#ifdef DEBUG
#define PROMPT MONAPI_TRUE
#else
#define PROMPT MONAPI_FALSE
#endif

using namespace MonAPI;

int MonaMain(List<char*>* pekoe)
{
	if (pekoe->size() < 1)
	{
		printf("usage: BZCAT FILENAME.BZ5\n");
		return 1;
	}
	
	monapi_cmemoryinfo* mi = monapi_call_file_decompress_st5_file(pekoe->get(0), PROMPT);
	if (mi == NULL) return 1;
	
	printf((const char*)mi->Data);
	monapi_cmemoryinfo_dispose(mi);
	monapi_cmemoryinfo_delete(mi);
	return 0;
}
