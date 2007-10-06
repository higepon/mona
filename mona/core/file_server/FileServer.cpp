#include "FileServer.h"

using namespace MonAPI;
using namespace std;

#define IRQ_PRIMARY   14
#define IRQ_SECONDARY 15

string upperCase(const string& s)
{
    string result = s;
    transform(result.begin(), result.end(), result.begin(), toupper);
    return result;
}

FileServer::FileServer()
{
    vmanager_ = new VnodeManager();
}

FileServer::~FileServer()
{
    delete vmanager_;
    delete rootFS_;
    for (FileSystems::const_iterator it = mountedFSs_.begin(); it != mountedFSs_.end(); ++it)
    {
        delete (*it);
    }
    cd_->close();
    delete cd_;
    delete fd_;
}

int FileServer::initializeFileSystems()
{
    if (initializeRootFileSystem() != MONA_SUCCESS)
    {
        _printf("initialize RootFileSystem error %s %s:%d\n", __func__, __FILE__, __LINE__);
        return MONA_FAILURE;
    }
    if (initializeMountedFileSystems() != MONA_SUCCESS)
    {
        _printf("initializeMountedFileSystems error %s %s:%d\n", __func__, __FILE__, __LINE__);
        return MONA_FAILURE;
    }
    vmanager_->setRoot(rootFS_->getRoot());
    return MONA_SUCCESS;
}

int FileServer::initializeMountedFileSystems()
{
    // ProcessFileSystem
#if 0
    FileSystem* pfs = new ProcessFileSystem(vmanager_);
    if (pfs->initialize() != MONA_SUCCESS)
    {
        _printf("ProcessFileSystem initialize Error\n");
        delete pfs;
        return MONA_FAILURE;
    }
    vmanager_->mount(rootFS_->getRoot(), "process", pfs->getRoot());
    mountedFSs_.push_back(pfs);
#endif

#if 0
    // FAT12FileSystem
    fd_ = new FDCDriver();
    FileSystem* ffs = new FAT12FileSystem(fd_, vmanager_);
    if (ffs->initialize() != MONA_SUCCESS)
    {
        _printf("Warning FAT12FileSystem initialize failed \n");
        delete fd_;
        delete ffs;
        return MONA_SUCCESS;
    }
    vmanager_->mount(rootFS_->getRoot(), "FD", ffs->getRoot());
    mountedFSs_.push_back(ffs);
#endif
    return MONA_SUCCESS;
}

int FileServer::initializeRootFileSystem()
{
//#if 1
    // user mode I/O
    syscall_get_io();

    // IDE Driver
    cd_ = new IDEDriver(IRQ_PRIMARY, IRQ_SECONDARY);

    // find CD-ROM
    int controller, deviceNo;
    if (!cd_->findDevice(IDEDriver::DEVICE_ATAPI, 0x05, &controller, &deviceNo))
    {
        _printf("CD-ROM Not Found\n");
        delete cd_;
        return MONA_FAILURE;
    }

    // set irq number
    uint8_t irq = controller == IDEDriver::PRIMARY ? IRQ_PRIMARY : IRQ_SECONDARY;

    // enable interrupts
    monapi_set_irq(irq, MONAPI_TRUE, MONAPI_TRUE);
    syscall_set_irq_receiver(irq, MONAPI_FALSE);

    // CD Select Device
    if (!cd_->selectDevice(controller, deviceNo))
    {
        _printf("select device NG error code = %d\n", cd_->getLastError());
        delete cd_;
        return MONA_FAILURE;
    }

    rootFS_ = new ISO9660FileSystem(cd_, vmanager_);
    if (rootFS_->initialize() != MONA_SUCCESS)
    {
        _printf("CD Boot Initialize Error\n");
        delete rootFS_;
        delete cd_;
        return MONA_FAILURE;
    }
// #endif
// #if 0
//     // FAT12FileSystem
//     fd_ = new FDCDriver();
//     rootFS_ = new FAT12FileSystem(fd_, vmanager_);
//     if (rootFS_->initialize() != MONA_SUCCESS)
//     {
//         _printf("FD Boot initialize failed \n");
//         delete fd_;
//         delete rootFS_;
//         return MONA_FAILURE;
//     }
// #endif
    return MONA_SUCCESS;
}

monapi_cmemoryinfo* FileServer::readFileAll(const string& file)
{
    uint32_t fileID;
    uint32_t tid = monapi_get_server_thread_id(ID_FILE_SERVER);
    int ret = vmanager_->open(file, 0, false, tid, &fileID);
    if (ret != MONA_SUCCESS) return NULL;

    Stat st;
    ret = vmanager_->stat(fileID, &st);
    if (ret != MONA_SUCCESS)
    {
        ret = vmanager_->close(fileID);
        return NULL;
    }

    monapi_cmemoryinfo* mi;
    ret = vmanager_->read(fileID, st.size, &mi);
    if (ret != MONA_SUCCESS)
    {
        ret = vmanager_->close(fileID);
        return NULL;
    }

    ret = vmanager_->close(fileID);
    return mi;
}

void FileServer::messageLoop()
{
    for (MessageInfo msg;;)
    {
        if (Message::receive(&msg)) continue;
        switch (msg.header)
        {
        case MSG_FILE_OPEN:
        {
            uint32_t tid = msg.from; // temporary
            uint32_t fildID;
            bool create = msg.arg1 == MONAPI_TRUE;
            int ret = vmanager_->open(upperCase(msg.str).c_str(), 0, create, tid, &fildID);
            Message::reply(&msg, ret == MONA_SUCCESS ? fildID : MONA_FAILURE);
            break;
        }
        case MSG_FILE_READ_ALL:
        {
            monapi_cmemoryinfo* mi = readFileAll(upperCase(msg.str).c_str());
            if (NULL == mi)
            {
                Message::reply(&msg, MONA_FAILURE);
            }
            else
            {
                uint32_t handle = mi->Handle;
                uint32_t size = mi->Size;
                monapi_cmemoryinfo_delete(mi);
                Message::reply(&msg, handle, size);
            }
            break;
        }
        case MSG_FILE_SEEK:
        {
            int ret = vmanager_->seek(msg.arg1 /* fileID */, msg.arg2 /* offset */, msg.arg3 /* origin */);
            Message::reply(&msg, ret == MONA_SUCCESS ? MONA_SUCCESS : MONA_FAILURE);
            break;
        }
        case MSG_FILE_READ:
        {
            uint32_t fileID = msg.arg1;
            monapi_cmemoryinfo* memory;
            int ret = vmanager_->read(fileID, msg.arg2 /* size */, &memory);
            if (ret != MONA_SUCCESS)
            {
                Message::reply(&msg, MONA_FAILURE);
            }
            else
            {
                uint32_t handle = memory->Handle;
                uint32_t size = memory->Size;
                monapi_cmemoryinfo_delete(memory);
                Message::reply(&msg, handle, size);
            }
            break;
        }
        case MSG_FILE_WRITE:
        {
            uint32_t fileID = msg.arg1;
            monapi_cmemoryinfo* memory;
            memory = monapi_cmemoryinfo_new();
            memory->Handle = msg.arg3;
            memory->Owner  = msg.from;
            memory->Size   = msg.arg2;
            monapi_cmemoryinfo_map(memory);
            int ret = vmanager_->write(fileID, msg.arg2 /* size */, memory);
//            monapi_cmemoryinfo_dispose(memory);
            monapi_cmemoryinfo_delete(memory);
            Message::reply(&msg, ret);
            break;
        }
        case MSG_FILE_CLOSE:
        {
            int ret = vmanager_->close(msg.arg1);
            Message::reply(&msg, ret == MONA_SUCCESS ? MONA_SUCCESS : MONA_FAILURE);
            break;
        }
        case MSG_FILE_GET_SIZE:
        {
            Stat st;
            int ret = vmanager_->stat(msg.arg1, &st);
            Message::reply(&msg, ret == MONA_SUCCESS ? MONA_SUCCESS : MONA_FAILURE, st.size);
            break;
        }
        case MSG_FILE_READ_DIRECTORY:
        {
            monapi_cmemoryinfo* memory;
            int ret = vmanager_->readdir(upperCase(msg.str).c_str(), &memory);
            if (ret != MONA_SUCCESS)
            {
                Message::reply(&msg, MONA_FAILURE);
            }
            else
            {
                uint32_t handle = memory->Handle;
                uint32_t size = memory->Size;
                monapi_cmemoryinfo_delete(memory);
                Message::reply(&msg, handle, size);
            }
            break;
        }
        case MSG_FILE_DECOMPRESS_ST5:
        {
            monapi_cmemoryinfo* mi1 = monapi_cmemoryinfo_new();
            mi1->Handle = msg.arg1;
            mi1->Size   = msg.arg2;
            monapi_cmemoryinfo* mi2 = NULL;
            if (monapi_cmemoryinfo_map(mi1))
            {
                mi2 = ST5Decompress(mi1);
                monapi_cmemoryinfo_dispose(mi1);
            }
            if (mi2 != NULL)
            {
                Message::reply(&msg, mi2->Handle, mi2->Size);
                monapi_cmemoryinfo_delete(mi2);
            }
            else
            {
                Message::reply(&msg);
            }
            monapi_cmemoryinfo_delete(mi1);
            break;
        }
        case MSG_FILE_DECOMPRESS_ST5_FILE:
        {
            monapi_cmemoryinfo* mi = ST5DecompressFile(upperCase(msg.str).c_str());
            if (mi != NULL)
            {
                Message::reply(&msg, mi->Handle, mi->Size);
                delete mi;
            }
            else
            {
                Message::reply(&msg);
            }
            break;
        }
        case MSG_STOP_SERVER:
            // end
            Message::reply(&msg, MONA_SUCCESS);
            return;
            break;
        case MSG_DISPOSE_HANDLE:
            MemoryMap::unmap(msg.arg1);
            Message::reply(&msg);
            break;
        default:
            PsInfo psInfo;
            syscall_set_ps_dump();
            while (syscall_read_ps_dump(&psInfo) == 0)
            {
                if (psInfo.tid == msg.from) break;
            }
//            _printf("%s %s:%d %d %x %s %x\n", __func__, __FILE__, __LINE__, msg.header, msg.from, psInfo.name, msg.arg1);
            break;
        }
    }
}

int64_t FileServer::GetST5DecompressedSize(monapi_cmemoryinfo* mi)
{
    int64_t ret = 0;
    ret = tek_checkformat(mi->Size, (unsigned char*)mi->Data);
    return ret;
}

monapi_cmemoryinfo* FileServer::ST5Decompress(monapi_cmemoryinfo* mi)
{
    int64_t size = GetST5DecompressedSize(mi);
    if (size < 0) return NULL;

    // if size >= 4GB abort...
    if ((size >> 32) > 0) return NULL;

    monapi_cmemoryinfo* ret = new monapi_cmemoryinfo();
    if (!monapi_cmemoryinfo_create(ret, (uint32_t)(size + 1), 0))
    {
        monapi_cmemoryinfo_delete(ret);
        return NULL;
    }
    ret->Size--;

    if (tek_decode(mi->Size, mi->Data, ret->Data) != 0) {
        // Decompress failed
        monapi_cmemoryinfo_dispose(ret);
        monapi_cmemoryinfo_delete(ret);
        return NULL;
    }

    ret->Data[ret->Size] = 0;
    return ret;
}

monapi_cmemoryinfo* FileServer::ST5DecompressFile(const char* file)
{
    monapi_cmemoryinfo* mi = readFileAll(file);
    monapi_cmemoryinfo* ret = NULL;
    if (mi == NULL) return ret;

    ret = ST5Decompress(mi);
    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);
    return ret;
}
