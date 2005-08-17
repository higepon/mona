#include <monapi.h>
#include <monapi/CString.h>
#include <monapi/messages.h>

using namespace MonAPI;

int MonaMain(List<char*>* argv)
{
	// 設定ファイル名
	CString filepath = System::getBundlePath();
	filepath += "/MONAPPT.INI";
	
	// ファイル読み込み
	printf("loading %s...\n", (const char*)filepath);
	monapi_cmemoryinfo* mi = monapi_call_file_read_data((const char*)filepath, MONAPI_FALSE);
	if (mi == NULL || mi->Size == 0) {
		printf("file read error\n");
		return(-1);
	}
	unsigned int filesize = mi->Size;
	unsigned char* filebuff = (unsigned char*)malloc(mi->Size);
	if (filebuff == NULL) {
		printf("memory allocate error\n");
		monapi_cmemoryinfo_dispose(mi);
		monapi_cmemoryinfo_delete(mi);
		return(-1);
	}
	memcpy(filebuff, mi->Data, mi->Size);
	monapi_cmemoryinfo_dispose(mi);
	monapi_cmemoryinfo_delete(mi);
	
	// ファイル解析
	HList<CString> list;
	unsigned char temp[20];
	memset(temp, '\0', sizeof(temp));
	for (int i = 0; i < (int)filesize; i++) {
		if (filebuff[i] == '\r') {
			// do nothing
		} else if (filebuff[i] == '\n' || i == (int)filesize - 1) {
			list.add((const char*)temp);
			memset(temp, '\0', sizeof(temp));
		} else {
			temp[strlen((const char*)temp)] = filebuff[i];
		}
	}
	for (int i = 0; i < list.size(); i++) {
		if (list.get(i).endsWith(".jpg")) {
			printf("/APPS/JPEGDEMO.EX5 %s\n", (const char*)list.get(i));
		} else if (list.get(i).endsWith(".mpg")) {
			printf("/APPS/MONAPEG.EX5 %s\n", (const char*)list.get(i));
		}
	}
	
	free(filebuff);
	return(0);
}
