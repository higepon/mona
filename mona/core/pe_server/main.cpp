#include <monapi.h>
#include <monapi/CString.h>
#include <monapi/messages.h>
#include <monalibc.h>

//#define NO_CACHE
//#define SEARCH_CURRENT

#include "PEServer.h"
#include "PEParser.h"

using namespace MonAPI;

typedef struct
{
	CString Name, Path;
	monapi_cmemoryinfo* Data;
	PEParser Parser;
} PEData;

static HList<PEData*> cache;

static PEData* GetPEData(const CString& path)
{
	int size = cache.size();
	for (int i = 0; i < size; i++)
	{
		PEData* pe = cache[i];
		if (pe->Path == path) return pe;
	}
	return NULL;
}

static PEData* OpenPE(const CString& path, bool prompt)
{
	PEData* ret = GetPEData(path);
	if (ret != NULL) return ret;
	
	int len = path.getLength();
	int p1 = path.lastIndexOf('/') + 1;
	int p2 = path.lastIndexOf('.');
	if (p1 > p2) return NULL;
	
	CString ext = path.substring(p2, len - p2).toUpper(), ext2;
	if (ext.startsWith(".EX"))
	{
		ext2 = ".EXE";
	}
	else if (ext.startsWith(".DL"))
	{
		ext2 = ".DLL";
	}
	else
	{
		return NULL;
	}
	
	ret = new PEData();
	ret->Name = path.substring(p1, p2 - p1) + ext2;
	ret->Path = path;
	
	if (path.endsWith("2"))
	{
		ret->Data = monapi_call_file_decompress_bz2_file(path, prompt);
	}
	else if (path.endsWith("5"))
	{
		ret->Data = monapi_call_file_decompress_st5_file(path, prompt);
	}
	else
	{
		ret->Data = monapi_file_read_all(path);
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
	
#ifndef NO_CACHE
	cache.add(ret);
#endif
	return ret;
}

class DLLManager
{
private:
	bool initialized;
	monapi_cmemoryinfo* files1;
	monapi_cmemoryinfo* files2;
	CString path;
	bool prompt;
	
public:
	DLLManager(const CString& path, bool prompt)
		: initialized(false), files1(NULL), files2(NULL), prompt(prompt)
	{
#ifdef SEARCH_CURRENT
		int p = path.lastIndexOf('/');
		if (p >= 0)
		{
			CString pn = p == 0 ? "/" : path.substring(0, p).toUpper();
			if (pn != DLLPATH) this->path = pn;
		}
#endif
	}
	
	~DLLManager()
	{
		if (this->files1 != NULL)
		{
			monapi_cmemoryinfo_dispose(this->files1);
			monapi_cmemoryinfo_delete(this->files1);
		}
		if (this->files2 != NULL)
		{
			monapi_cmemoryinfo_dispose(this->files2);
			monapi_cmemoryinfo_delete(this->files2);
		}
	}
	
private:
	bool IsReady()
	{
		return this->files1 != NULL || this->files2 != NULL;
	}
	
	bool Init()
	{
		if (this->path != NULL)
		{
			this->files1 = monapi_file_read_directory(this->path);
		}
		this->files2 = monapi_file_read_directory(DLLPATH);
		this->initialized = true;
		return this->IsReady();
	}
	
	CString Find(monapi_cmemoryinfo* files, const CString& path, const CString& dll)
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
	
public:
	CString Find(CString dll)
	{
		if (!this->initialized && !this->Init())
		{
			if (this->prompt) printf("%s: can not find DLL path!\n", SVR);
			return NULL;
		}
		else if (!this->IsReady())
		{
			return NULL;
		}
		
		if (dll.endsWith(".DLL")) dll = dll.substring(0, dll.getLength() - 4);
		
		CString ret = this->Find(this->files1, this->path, dll);
		if (ret != NULL) return ret;
		
		ret = this->Find(this->files2, DLLPATH, dll);
		if (ret == NULL)
		{
			if (this->prompt) printf("%s: can not find: %s.DLL\n", SVR, (const char*)dll);
		}
		return ret;
	}
};

class PELinker
{
private:
	DLLManager dllmgr;
	bool prompt;
	HList<PEData*> list;
	
public:
	dword EntryPoint;
	monapi_cmemoryinfo* Binary;
	int Result;
	
	PELinker(const CString& path, bool prompt)
		: dllmgr(path, prompt), prompt(prompt), EntryPoint(0), Binary(NULL), Result(0)
	{
		if (!this->Open(path)) return;
		
		PEData* exe = this->list[0];
		this->EntryPoint = exe->Parser.get_EntryPoint();
		this->Load();
	}
	
	~PELinker()
	{
		if (this->Binary != NULL) monapi_cmemoryinfo_delete(this->Binary);
#ifdef NO_CACHE
		int len = this->list.size();
		for (int i = 0; i < len; i++)
		{
			PEData* data = this->list[i];
			monapi_cmemoryinfo_dispose(data->Data);
			monapi_cmemoryinfo_delete(data->Data);
			delete data;
		}
#endif
	}
	
private:
	PEData* Find(const CString& name)
	{
		int len = this->list.size();
		for (int i = 0; i < len; i++)
		{
			PEData* data = this->list[i];
			if (data->Name == name) return data;
		}
		return NULL;
	}
	
	bool Open(const CString& path)
	{
		if (path == NULL)
		{
			this->Result = 1;
			return false;
		}
		
		int len = this->list.size();
		for (int i = 0; i < len; i++)
		{
			if (this->list[i]->Path == path) return true;
		}
		
		PEData* pe = OpenPE(path, this->prompt);
		if (pe == NULL)
		{
			this->Result = 1;
			return false;
		}
		else if (pe->Data == NULL)
		{
#ifdef NO_CACHE
			delete pe;
#endif
			this->Result = 3;
			return false;
		}
		
		this->list.add(pe);
		
		HList<CString> dlls;
		int its = pe->Parser.get_ImportTableCount();
		for (int i = 0; i < its; i++)
		{
			CString name = CString(pe->Parser.GetImportTableName(i)).toUpper();
			if (!name.endsWith(".DLL"))
			{
				if (this->prompt) printf("%s: specified dll is not valid: %s\n", SVR, (const char*)name);
				this->Result = 1;
				return false;
			}
			else if (this->Find(name) != NULL)
			{
				continue;
			}
			else if (!this->Open(dllmgr.Find(name)))
			{
				return false;
			}
		}
		return true;
	}
	
	bool Load()
	{
		int imageSize = 0;
		int len = this->list.size();
		for (int i = 0; i < len; i++)
		{
			imageSize += this->list[i]->Parser.get_ImageSize();
		}
		
		monapi_cmemoryinfo* dst = monapi_cmemoryinfo_new();
		if (!monapi_cmemoryinfo_create(dst, imageSize, this->prompt))
		{
			monapi_cmemoryinfo_delete(dst);
			this->Result = 3;
			return false;
		}

		uint32_t addr = 0;
		for (int i = 0; i < len; i++)
		{
			PEData* data = this->list[i];
			uint8_t* ptr = &dst->Data[addr];
			if (!data->Parser.Load(ptr))
			{
				if (this->prompt) printf("%s: can not load: %s\n", SVR, (const char*)data->Name);
#ifdef NO_CACHE
				monapi_cmemoryinfo_dispose(dst);
				monapi_cmemoryinfo_delete(dst);
#endif
				this->Result = 3;
				return false;
			}
			if (i > 0 && !data->Parser.Relocate(ptr, ORG + addr))
			{
				if (this->prompt) printf("%s: can not relocate: %s\n", SVR, (const char*)data->Name);
#ifdef NO_CACHE
				monapi_cmemoryinfo_dispose(dst);
				monapi_cmemoryinfo_delete(dst);
#endif
				this->Result = 3;
				return false;
			}
			addr += data->Parser.get_ImageSize();
		}

		addr = 0;
		for (int i = 0; i < len; i++)
		{
			PEData* data = this->list[i];
			int its = data->Parser.get_ImportTableCount();
			for (int j = 0; j < its; j++)
			{
				CString dll = CString(data->Parser.GetImportTableName(j)).toUpper();
				if (this->prompt) printf("%s: Linking %s to %s....", SVR, (const char*)dll, (const char*)data->Name);
				PEData* target = this->Find(dll);
				if (target != NULL && data->Parser.Link(&dst->Data[addr], j, &target->Parser))
				{
					if (this->prompt) printf("OK\n");
				}
				else
				{
					if (this->prompt)
					{
						printf("NG\n");
						printf("%s: can not link %s to %s!\n", SVR, (const char*)dll, (const char*)data->Name);
					}
#ifdef NO_CACHE
					monapi_cmemoryinfo_dispose(dst);
					monapi_cmemoryinfo_delete(dst);
#endif
					this->Result = 3;
					return false;
				}
			}
			addr += data->Parser.get_ImageSize();
		}
		
		this->Binary = dst;
		return true;
	}
};

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
				PELinker pe(msg.str, msg.arg1 == MONAPI_TRUE);
				if (pe.Result == 0)
				{
					char buf[16];
					sprintf(buf, "%d", pe.Binary->Size);
					Message::reply(&msg, pe.Binary->Handle, pe.EntryPoint, buf);
				}
				else
				{
					Message::reply(&msg, 0, pe.Result);
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
