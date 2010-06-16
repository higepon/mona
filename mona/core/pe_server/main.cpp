#include <monapi.h>
#include <monapi/CString.h>
#include <monapi/messages.h>
#include <monalibc.h>
#include <vector>
#include <algorithm>

#define NO_CACHE
//#define SEARCH_CURRENT

#include "PEServer.h"
#include "PEParser.h"

using namespace MonAPI;
using namespace std;

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
        if (prompt) _printf("%s: file is not valid PE: %s\n", SVR, (const char*)path);
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
            if (this->prompt) _printf("%s: can not find DLL path!\n", SVR);
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
            if (this->prompt) _printf("%s: can not find: %s.DLL\n", SVR, (const char*)dll);
        }
        return ret;
    }
};

class PELinker
{
private:
    DLLManager dllmgr;
    bool prompt;
    typedef vector<PEData*> PEDataList;
    PEDataList list;
    PEDataList dlls;
    uint32_t bootstrapEntryPoint;

public:
    uint32_t EntryPoint;
    monapi_cmemoryinfo* Binary;
    int Result;

    PELinker(const CString& path, bool prompt)
        : dllmgr(path, prompt), prompt(prompt), EntryPoint(0), Binary(NULL), Result(0)
    {
        if (!this->Open(path)) return;
        dlls = list;
        dlls.erase(dlls.begin());
        reverse(dlls.begin(), dlls.end());

//        PEData* exe = this->list[0];
//        this->EntryPoint = exe->Parser.get_EntryPoint();
        this->Load();
        this->EntryPoint = bootstrapEntryPoint;
    }

    ~PELinker()
    {
        if (this->Binary != NULL) monapi_cmemoryinfo_delete(this->Binary);
#ifdef NO_CACHE
        PEDataList::size_type len = this->list.size();
        for (PEDataList::size_type i = 0; i < len; i++)
        {
            PEData* data = this->list[i];
            monapi_cmemoryinfo_dispose(data->Data);
            monapi_cmemoryinfo_delete(data->Data);
            delete data;
        }
#endif
    }

private:

    uint32_t getBootstrapSize()
    {
        uint32_t ret = 13;
        for (PEDataList::const_iterator it = list.begin(); it != list.end(); ++it)
        {
            PEData* pe = *it;
            uint32_t entryPoint = pe->Parser.get_EntryPoint();
            if (0 == entryPoint) continue;
            ret += 13 * 2;
        }
        return ret;
    }

    PEData* Find(const CString& name)
    {
        PEDataList::size_type len = this->list.size();
        for (PEDataList::size_type i = 0; i < len; i++)
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

        PEDataList::size_type len = this->list.size();
        for (PEDataList::size_type i = 0; i < len; i++)
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

        this->list.push_back(pe);

        HList<CString> dlls;
        int its = pe->Parser.get_ImportTableCount();
        for (int i = 0; i < its; i++)
        {
            CString name = CString(pe->Parser.GetImportTableName(i)).toUpper();
            if (!name.endsWith(".DLL"))
            {
                if (this->prompt) _printf("%s: specified dll is not valid: %s\n", SVR, (const char*)name);
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
        uint32_t imageSize = 0;
        uint32_t bootstrapSize = getBootstrapSize();
        PEDataList::size_type len = this->list.size();
        for (PEDataList::size_type i = 0; i < len; i++)
        {
            imageSize += this->list[i]->Parser.get_ImageSize();
        }

        imageSize += bootstrapSize;

        monapi_cmemoryinfo* dst = monapi_cmemoryinfo_new();
        if (monapi_cmemoryinfo_create(dst, imageSize, this->prompt) != M_OK)
        {
            monapi_cmemoryinfo_delete(dst);
            this->Result = 3;
            return false;
        }
        uint32_t addr = 0;
        for (PEDataList::size_type i = 0; i < len; i++)
        {
            PEData* data = this->list[i];
            uint8_t* ptr = &dst->Data[addr];
            if (!data->Parser.Load(ptr))
            {
                if (this->prompt) _printf("%s: can not load: %s\n", SVR, (const char*)data->Name);
#ifdef NO_CACHE
                monapi_cmemoryinfo_dispose(dst);
                monapi_cmemoryinfo_delete(dst);
#endif
                this->Result = 3;
                return false;
            }
            if (i > 0 && !data->Parser.Relocate(ptr, ORG + addr))
            {
                if (this->prompt) _printf("%s: can not relocate: %s\n", SVR, (const char*)data->Name);
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
        for (PEDataList::size_type i = 0; i < len; i++)
        {
            PEData* data = this->list[i];
            int its = data->Parser.get_ImportTableCount();
            for (int j = 0; j < its; j++)
            {
                CString dll = CString(data->Parser.GetImportTableName(j)).toUpper();
                PEData* target = this->Find(dll);

                if (target == NULL || !data->Parser.Link(&dst->Data[addr], j, &target->Parser))
                {
                    if (this->prompt)
                    {
                        _printf("NG\n");
                        _printf("%s: can not link %s to %s!\n", SVR, (const char*)dll, (const char*)data->Name);
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
        // make bootstrap code
        uint8_t* bootstrap = &dst->Data[dst->Size - bootstrapSize];
        uint8_t* start = &bootstrap[3];
        bootstrap[0] = 0x55; // push ebp
        bootstrap[1] = 0x89; // mov ebp
        bootstrap[2] = 0xE5; // ,esp
        PEDataList::size_type i;
        //char buf[128];
        const uint32_t CALL_CODE_SIZE = 13;

        // dllmain construct
        for (i = 0; i < dlls.size(); i++)
        {
            PEData* dll = dlls[i];
            start[i * CALL_CODE_SIZE] = 0xB8; // mov eax
            *(uint32_t*)(&start[i * CALL_CODE_SIZE + 1]) = dll->Parser.get_EntryPoint(); // ,address
            start[i * CALL_CODE_SIZE + 5] = 0x68; // push
            *(uint32_t*)(&start[i * CALL_CODE_SIZE + 6]) = 0; // 0x00000000 = argument of dllmain
            start[i * CALL_CODE_SIZE + 10] = 0xFF; // call eax
            start[i * CALL_CODE_SIZE + 11] = 0xD0; // call eax
            start[i * CALL_CODE_SIZE + 12] = 0x58; // pop eax
        }
        PEData* exe = list[0];
        start[i * CALL_CODE_SIZE + 0] = 0xB8; // mov eax
        *(uint32_t*)(&start[i * CALL_CODE_SIZE + 1]) = exe->Parser.get_EntryPoint();
        start[i * CALL_CODE_SIZE + 5] = 0xFF;
        start[i * CALL_CODE_SIZE + 6] = 0xD0;
        start = &start[i * CALL_CODE_SIZE + 7];

        // destruct
        for (i = 0; i < dlls.size(); i++)
        {
            PEData* dll = dlls[dlls.size() - i - 1];
            start[i * CALL_CODE_SIZE] = 0xB8; // mov eax
            *(uint32_t*)(&start[i * CALL_CODE_SIZE + 1]) = dll->Parser.get_EntryPoint(); // ,address
            start[i * CALL_CODE_SIZE + 5] = 0x68; // push
            *(uint32_t*)(&start[i * CALL_CODE_SIZE + 6]) = 1; // 1 = argument of dllmain
            start[i * CALL_CODE_SIZE + 10] = 0xFF; // call eax
            start[i * CALL_CODE_SIZE + 11] = 0xD0; // call eax
            start[i * CALL_CODE_SIZE + 12] = 0x58; // pop eax
        }
        start[i * CALL_CODE_SIZE] = 0xC9;
        start[i * CALL_CODE_SIZE + 1] = 0xC3;

        bootstrapEntryPoint = dst->Size - bootstrapSize + ORG;
        // make bootstrap end
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

int main(int argc, char* argv[])
{
    if (monapi_notify_server_start("MONITOR.BIN") != M_OK)
    {
        exit(-1);
    }

    MessageLoop();

    return 0;
}
