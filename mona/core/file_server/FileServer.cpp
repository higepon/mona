
#include "FileServer.h"
#include "ram_disk/RamDisk.h"
#include "FAT32/Fat.h"

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
}

FileServer::~FileServer()
{
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
    int ret = initializeRootFileSystem();
    if (ret != M_OK) {
        monapi_fatal("initialize RootFileSystem error %s %s:%d\n", __func__, __FILE__, __LINE__);
        return ret;
    }
    ret = initializeMountedFileSystems();
    if (ret != M_OK) {
        monapi_fatal("initializeMountedFileSystems error %s %s:%d\n", __func__, __FILE__, __LINE__);
        return ret;
    }
    vmanager_.setRoot(rootFS_->getRoot());
    return M_OK;
}

int FileServer::initializeMountedFileSystems()
{
    bd4_ = new BlockDeviceDriver(4);
    FatFileSystem* fatfs = new FatFileSystem(vmanager_, *bd4_);
    if (fatfs->initialize() != M_OK) {
        monapi_warn("Fat fs mound error");
        delete fatfs;
        return M_OK;
    }
    vmanager_.mount(rootFS_->getRoot(), "USER", fatfs->getRoot());
    mountedFSs_.push_back(fatfs);

    // RamDiskFileSystem
    RamDisk::RamDiskFileSystem* rdf = new RamDisk::RamDiskFileSystem(&vmanager_);
    if (rdf->initialize() != M_OK) {
        monapi_warn("Warning RamDisk file system initialize failed \n");
        delete rdf;
        return M_OK;
    }
    vmanager_.mount(rootFS_->getRoot(), "MEM", rdf->getRoot());
    mountedFSs_.push_back(rdf);

    FatFileSystem* fatfs = new FatFileSystem(vmanager_);
    if (rdf->initialize() != M_OK)
    {
        _printf("Warning RamDisk file system initialize failed \n");
        delete rdf;
        return M_OK;
    }
    vmanager_->mount(rootFS_->getRoot(), "MEM", rdf->getRoot());
    mountedFSs_.push_back(rdf);


    return M_OK;
}

int FileServer::initializeRootFileSystem()
{
//#if 1
    // user mode I/O
    syscall_get_io();


#if 0
    // IDE Driver
    cd_ = new IDEDriver(IRQ_PRIMARY, IRQ_SECONDARY);

    // find CD-ROM
    int controller, deviceNo;
    if (!cd_->findDevice(IDEDriver::DEVICE_ATAPI, 0x05, &controller, &deviceNo))
    {
        monapi_warn("CD-ROM Not Found\n");
        delete cd_;
        return M_OK;
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
        return M_BAD_ARG;
    }
    rootFS_ = new ISO9660FileSystem(cd_, vmanager_);
#else
    bd_ = new BlockDeviceDriver(0, 2048);
    rootFS_ = new ISO9660FileSystem(bd_, &vmanager_);
#endif

    int ret = rootFS_->initialize();
    if (ret != M_OK)
    {
        monapi_warn("CD Boot Initialize Error\n");
        delete rootFS_;
        delete cd_;
        return ret;
    }
    return M_OK;
}

monapi_cmemoryinfo* FileServer::readFileAll(const string& file)
{
    uint32_t fileID;
    uint32_t tid = monapi_get_server_thread_id(ID_FILE_SERVER);
    int ret = vmanager_.open(file, 0, tid, &fileID);
    if (ret != M_OK) return NULL;

    Stat st;
    ret = vmanager_.stat(fileID, &st);
    if (ret != M_OK)
    {
        ret = vmanager_.close(fileID);
        return NULL;
    }

    monapi_cmemoryinfo* mi;
    ret = vmanager_.read(fileID, st.size, &mi);
    if (ret != M_OK)
    {
        ret = vmanager_.close(fileID);
        return NULL;
    }

    ret = vmanager_.close(fileID);
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
            intptr_t mode = msg.arg1;
            int ret = vmanager_.open(upperCase(msg.str).c_str(), mode, tid, &fildID);
            Message::reply(&msg, ret == M_OK ? fildID : M_FILE_NOT_FOUND);
            break;
        }
        case MSG_FILE_READ_ALL:
        {
            monapi_cmemoryinfo* mi = readFileAll(upperCase(msg.str).c_str());
            if (NULL == mi)
            {
                Message::reply(&msg, M_READ_ERROR);
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
            int ret = vmanager_.seek(msg.arg1 /* fileID */, msg.arg2 /* offset */, msg.arg3 /* origin */);
            Message::reply(&msg, ret);
            break;
        }
        case MSG_FILE_READ:
        {
            uint32_t fileID = msg.arg1;
            monapi_cmemoryinfo* memory;
            int ret = vmanager_.read(fileID, msg.arg2 /* size */, &memory);
            if (ret != M_OK)
            {
                Message::reply(&msg, ret);
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
            // Use immediate map
            intptr_t result = monapi_cmemoryinfo_map(memory, true);
            if (result != M_OK) {
                monapi_cmemoryinfo_delete(memory);
                Message::replyError(&msg, result);
            } else {
                int ret = vmanager_.write(fileID, msg.arg2 /* size */, memory);

                // We are sure that clients don't want to be notified with MSG_DISPOSE_HANDLE.
                if (monapi_cmemoryinfo_dispose_no_notify(memory) != M_OK) {
                    logprintf("[Warning] FileServer: MSG_FILE_WRITE. monapi_cmemoryinfo_dispose_no_notify error memory->Handle=%x\n", memory->Handle);
                }
                monapi_cmemoryinfo_delete(memory);
                Message::reply(&msg, ret);
            }
            break;
        }
        case MSG_FILE_CLOSE:
        {
            int ret = vmanager_.close(msg.arg1);
            Message::reply(&msg, ret);
            break;
        }
        case MSG_FILE_GET_SIZE:
        {
            Stat st;
            int ret = vmanager_.stat(msg.arg1, &st);
            Message::reply(&msg, ret, st.size);
            break;
        }
        case MSG_FILE_READ_DIRECTORY:
        {
            monapi_cmemoryinfo* memory;
            int ret = vmanager_.readdir(upperCase(msg.str).c_str(), &memory);
            if (ret != M_OK) {
                Message::reply(&msg, ret);
            } else {
                uint32_t handle = memory->Handle;
                uint32_t size = memory->Size;
                monapi_cmemoryinfo_delete(memory);
                Message::reply(&msg, handle, size);
            }
            break;
        }
        case MSG_FILE_DELETE:
        {
            int ret = vmanager_.delete_file(upperCase(msg.str).c_str());
            Message::reply(&msg, ret);
            break;
        }
        case MSG_FILE_DECOMPRESS_ST5:
        {
            monapi_cmemoryinfo* mi1 = monapi_cmemoryinfo_new();
            mi1->Handle = msg.arg1;
            mi1->Size   = msg.arg2;
            monapi_cmemoryinfo* mi2 = NULL;
            if (monapi_cmemoryinfo_map(mi1, true) != M_OK)
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
            Message::reply(&msg, M_OK);
            return;
            break;
        case MSG_DISPOSE_HANDLE:
            MemoryMap::unmap(msg.arg1);
            Message::reply(&msg);
            break;
        default:
//             PsInfo psInfo;
//             syscall_set_ps_dump();
//             while (syscall_read_ps_dump(&psInfo) == 0)
//             {
//                 if (psInfo.tid == msg.from) {
//                     monapi_warn("unknown message %x comes from %s arg1=%x %s:%d %s\n", msg.header, psInfo.name, msg.arg1, __FILE__, __LINE__,  __func__);
//                 }
//             }
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
    if (monapi_cmemoryinfo_create(ret, (uint32_t)(size + 1), 0, true) != M_OK)
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
