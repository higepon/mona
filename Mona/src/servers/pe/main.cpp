#include <monapi.h>
#include <monapi/CString.h>
#include <monapi/messages.h>
#include <string>
#include <map>
#include "PEServer.h"
#include "PEParser.h"

using namespace MonAPI;

typedef struct
{
	CString Name;
	monapi_cmemoryinfo* Data;
	PEParser Parser;
} PEData;

static std::map<std::string, monapi_cmemoryinfo*> cache;

static PEData* OpenPE(const CString& path, bool prompt)
{
	PEData* ret = new PEData();
	int len = path.getLength();
	int p1 = path.lastIndexOf('/');
	int p2 = path.lastIndexOf('.');
	if (p2 < p1) p2 = len;
	ret->Name = p1 < 0 ? path : path.substring(p1 + 1, p2 - (p1 + 1));
	ret->Data = cache[(const char*)path];
	
	if (ret->Data == NULL)
	{
		if (path.endsWith(".EX2") || path.endsWith(".DL2"))
		{
			ret->Data = monapi_call_file_decompress_bz2_file(path, prompt);
		}
		else if (path.endsWith(".EX5") || path.endsWith(".DL5"))
		{
			ret->Data = monapi_call_file_decompress_st5_file(path, prompt);
		}
		else
		{
			ret->Data = monapi_call_file_read_data(path, prompt);
		}
		cache[(const char*)path] = ret->Data;
	}
	if (ret->Data == NULL)
	{
		delete ret;
		return NULL;
	}
	
	if (!ret->Parser.Parse(ret->Data->Data, ret->Data->Size))
	{
		if (prompt) printf("%s: file is not valid PE: %s\n", SVR, (const char*)path);
#ifdef NO_CACHE
		monapi_cmemoryinfo_dispose(ret->Data);
		monapi_cmemoryinfo_delete(ret->Data);
#endif
		ret->Data = NULL;
	}
	
	return ret;
}

static int OpenDLLs(HList<PEData*>* list, HList<CString>* files, bool prompt)
{
	int len = files->size();
	for (int i = 0; i < len; i++)
	{
		PEData* data = OpenPE(files->get(i), prompt);
		if (data == NULL)
		{
			return 1;
		}
		else if (data->Data == NULL)
		{
			delete data;
			return 3;
		}
		
		list->add(data);
	}
	return 0;
}

static CString SearchDLL(const CString& path, monapi_cmemoryinfo* files, const CString& dll)
{
	if (files == NULL) return NULL;
	
	int size = *(int*)files->Data;
	monapi_directoryinfo* p = (monapi_directoryinfo*)&files->Data[sizeof(int)];
	for (int i = 0; i < size; i++, p++)
	{
		CString n = CString(p->name).toUpper();
		if (dll + ".DLL" == n || dll + ".DL2" == n || dll + ".DL5" == n)
		{
			return path == "/" ? "/" + n : path + "/" + n;
		}
	}
	return NULL;
}

static bool SearchDLLs(HList<CString>* dllfiles, HList<CString>* dlls, const CString& path, bool prompt)
{
	monapi_cmemoryinfo* files1 = NULL;
	monapi_cmemoryinfo* files2 = monapi_call_file_read_directory(DLLPATH, MONAPI_FALSE);
	CString pathName;
	int p = path.lastIndexOf('/');
	if (p >= 0)
	{
		CString pn = p == 0 ? "/" : path.substring(0, p).toUpper();
		if (pn != DLLPATH)
		{
			files1 = monapi_call_file_read_directory(pn, MONAPI_FALSE);
			pathName = pn;
		}
	}
	if (files1 == NULL && files2 == NULL)
	{
		if (prompt) printf("%s: can not find DLL path!\n", SVR);
		return false;
	}
	
	bool ret = true;
	int len = dlls->size();
	for (int i = 0; i < len; i++)
	{
		CString dll = dlls->get(i);
		CString dllfile = SearchDLL(pathName, files1, dll);
		if (dllfile == NULL) dllfile = SearchDLL(DLLPATH, files2, dll);
		if (dllfile == NULL)
		{
			if (prompt) printf("%s: can not find: %s.DLL\n", SVR, (const char*)dll);
			ret = false;
		}
		else
		{
			dllfiles->add(dllfile);
		}
	}
	
	if (files1 != NULL)
	{
		monapi_cmemoryinfo_dispose(files1);
		monapi_cmemoryinfo_delete(files1);
	}
	if (files2 != NULL)
	{
		monapi_cmemoryinfo_dispose(files2);
		monapi_cmemoryinfo_delete(files2);
	}
	return ret;
}

static bool ListUpDLL(HList<CString>* dlls, PEParser* parser, bool prompt)
{
	int its = parser->get_ImportTableCount();
	for (int i = 0; i < its; i++)
	{
		CString dll = parser->GetImportTableName(i);
		if (dll == NULL)
		{
			if (prompt) printf("%s: file is not valid PE!\n", SVR);
			return false;
		}
		else if (!dll.toUpper().endsWith(".DLL"))
		{
			if (prompt) printf("%s: specified dll is not valid: %s\n", SVR, (const char*)dll);
			return false;
		}
		CString name = dll.substring(0, dll.getLength() - 4).toUpper();
		bool exists = false;
		int len = dlls->size();
		for (int i = 0; i < len; i++)
		{
			if (dlls->get(i) == name)
			{
				exists = true;
				break;
			}
		}
		if (!exists) dlls->add(name);
	}
	return true;
}

static int LoadPE(HList<PEData*>* list, monapi_cmemoryinfo** dest, const CString& path, bool prompt)
{
	HList<CString> dlls, dllfiles;
	if (!ListUpDLL(&dlls, &list->get(0)->Parser, prompt)
		|| !SearchDLLs(&dllfiles, &dlls, path, prompt)
		|| OpenDLLs(list, &dllfiles, prompt) != 0) return 3;
	
	int imageSize = 0;
	int len = list->size();
	for (int i = 0; i < len; i++)
	{
		PEData* data = list->get(i);
		imageSize += data->Parser.get_ImageSize();
	}
	
	monapi_cmemoryinfo* dst = monapi_cmemoryinfo_new();
	if (!monapi_cmemoryinfo_create(dst, imageSize, prompt))
	{
		monapi_cmemoryinfo_delete(dst);
		return 3;
	}
	
	uint32_t addr = 0;
	for (int i = 0; i < len; i++)
	{
		PEData* data = list->get(i);
		data->Name += i == 0 ? ".EXE" : ".DLL";
		uint8_t* ptr = &dst->Data[addr];
		if (!data->Parser.Load(ptr))
		{
			if (prompt) printf("%s: can not load: %s\n", SVR, (const char*)data->Name);
#ifdef NO_CACHE
			monapi_cmemoryinfo_dispose(dst);
			monapi_cmemoryinfo_delete(dst);
#endif
			return 3;
		}
		if (i > 0 && !data->Parser.Relocate(ptr, ORG + addr))
		{
			if (prompt) printf("%s: can not relocate: %s\n", SVR, (const char*)data->Name);
#ifdef NO_CACHE
			monapi_cmemoryinfo_dispose(dst);
			monapi_cmemoryinfo_delete(dst);
#endif
			return 3;
		}
		addr += data->Parser.get_ImageSize();
	}
	
	addr = 0;
	for (int i = 0; i < len; i++)
	{
		PEData* data = list->get(i);
		int its = data->Parser.get_ImportTableCount();
		for (int j = 0; j < its; j++)
		{
			CString dll = CString(data->Parser.GetImportTableName(j)).toUpper();
			if (prompt) printf("%s: Linking %s to %s....", SVR, (const char*)dll, (const char*)data->Name);
			PEParser* target = NULL;
			for (int k = 0; k < len; k++)
			{
				if (list->get(k)->Name == dll)
				{
					target = &list->get(k)->Parser;
					break;
				}
			}
			if (data->Parser.Link(&dst->Data[addr], j, target))
			{
				if (prompt) printf("OK\n");
			}
			else
			{
				if (prompt)
				{
					printf("NG\n");
					printf("%s: can not link %s to %s!\n", SVR, (const char*)dll, (const char*)data->Name);
				}
#ifdef NO_CACHE
				monapi_cmemoryinfo_dispose(dst);
				monapi_cmemoryinfo_delete(dst);
#endif
				return 3;
			}
		}
		addr += data->Parser.get_ImageSize();
	}
	
	*dest = dst;
	return 0;
}

static int CreateImage(monapi_cmemoryinfo** dest, dword* entryPoint, const CString& path, bool prompt)
{
	PEData* exe = OpenPE(path, prompt);
	if (exe == NULL)
	{
		return 1;
	}
	else if (exe->Data == NULL)
	{
		delete exe;
		return 3;
	}
	
	*entryPoint = exe->Parser.get_EntryPoint();
	HList<PEData*> list;
	list.add(exe);
	monapi_cmemoryinfo* img;
	int result = LoadPE(&list, &img, path, prompt);
	int len = list.size();
	for (int i = 0; i < len; i++)
	{
		PEData* data = list.get(i);
#ifdef NO_CACHE
		monapi_cmemoryinfo_dispose(data->Data);
		monapi_cmemoryinfo_delete(data->Data);
#endif
		delete data;
	}
	if (result == 0) *dest = img;
	return result;
}

static void MessageLoop()
{
	for (MessageInfo msg;;)
	{
		if (Message::receive(&msg) != 0) continue;

		switch (msg.header)
		{
			case MSG_DISPOSE_HANDLE:
				MemoryMap::unmap(msg.arg1);
				Message::reply(&msg);
				break;
			case MSG_PROCESS_CREATE_IMAGE:
			{
				monapi_cmemoryinfo* mi = NULL;
				dword entryPoint = 0;
				int result = CreateImage(&mi, &entryPoint, msg.str, msg.arg1 == MONAPI_TRUE);
				if (result == 0)
				{
					char buf[16];
					sprintf(buf, "%d", mi->Size);
					Message::reply(&msg, mi->Handle, entryPoint, buf);
					monapi_cmemoryinfo_delete(mi);
				}
				else
				{
					Message::reply(&msg, 0, result);
				}
				break;
			}
			default:
				break;
		}
	}
}

int MonaMain(List<char*>* pekoe)
{
	if (Message::send(Message::lookupMainThread("MONITOR.BIN"), MSG_SERVER_START_OK) != 0)
	{
		printf("%s: MONITOR error\n", SVR);
		exit(1);
	}

	MessageLoop();

	return 0;
}
