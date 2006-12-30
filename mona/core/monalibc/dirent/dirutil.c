#include <monapi/messages.h>
#include <monapi/cmemoryinfo.h>
#include "dirent_p.h"

struct dirent* convertFromDirInfo(monapi_directoryinfo* di, struct dirent* ent)
{
	if( ent == NULL || di == NULL ) return NULL;
	ent->d_fileno = -1;
	ent->d_off = 0;
	ent->d_reclen = sizeof(monapi_directoryinfo);
	ent->d_type = di->attr;
	strlcpy(ent->d_name, di->name, 255);
	return ent;
}

monapi_directoryinfo *getDirInfo(monapi_cmemoryinfo* cmi, int index)
{
	int num;
	int sc;
	num = *cmi->Data;
	if( index >= num ) return NULL;
	sc = sizeof(monapi_directoryinfo)*index+sizeof(int);
	return (monapi_directoryinfo*)&cmi->Data[sc];
}

int getDirInfoNum(monapi_cmemoryinfo* cmi)
{
	return *cmi->Data;
}
