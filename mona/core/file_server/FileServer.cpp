
#include <monapi.h>
#include "FileServer.h"
#include "ram_disk/RamDisk.h"
#include "FAT32/Fat.h"

using namespace MonAPI;
using namespace std;

#define IRQ_PRIMARY   14
#define IRQ_SECONDARY 15

string upperCase(const string& s)
{
    string result;
    result = s;
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
    RamDisk::RamDiskFileSystem* rdf = new RamDisk::RamDiskFileSystem();
    if (rdf->initialize() != M_OK) {
        monapi_warn("Warning RamDisk file system initialize failed \n");
        delete rdf;
        return M_OK;
    }
    vmanager_.mount(rootFS_->getRoot(), "MEM", rdf->getRoot());
    mountedFSs_.push_back(rdf);

    bd4_ = new BlockDeviceDriver(4);
    FatFileSystem* fatfs = new FatFileSystem(*bd4_);
    if (fatfs->initialize() != M_OK) {
        monapi_warn("Fat fs mound error");
        delete fatfs;
        return M_OK;
    }
    vmanager_.mount(rootFS_->getRoot(), "USER", fatfs->getRoot());
    mountedFSs_.push_back(fatfs);

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
    rootFS_ = new ISO9660FileSystem(bd_);
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

SharedMemory * FileServer::readFileAll(const string& file, intptr_t& lastError)
{
    uint32_t fileID;
    uint32_t tid;
    if (monapi_name_whereis("/servers/file", tid) != M_OK) {
        return NULL;
    }
    int ret = vmanager_.open(file, 0, tid, &fileID);
    if (ret != M_OK) {
        lastError = ret;
        return NULL;
    }

    Stat st;
    ret = vmanager_.stat(fileID, &st);
    if (ret != M_OK) {
        monapi_warn("stat failed %d\n", ret);
        lastError = ret;
        vmanager_.close(fileID);
        return NULL;
    }
    SharedMemory* shm;
    ret = vmanager_.read(fileID, st.size, &shm);
    if (ret != M_OK) {
        monapi_warn("read failed %d\n", ret);
        lastError = ret;
        vmanager_.close(fileID);
        return NULL;
    }
    lastError = vmanager_.close(fileID);
    return shm;
}

void FileServer::send_and_release_shm(SharedMemory* mi, MessageInfo* msg)
{
    // To prevent miss freeing of shared map, waits the client notification.
    int ret = Message::sendReceive(msg, msg->from, MSG_OK, msg->header, mi->handle(), mi->size());
    if (ret != M_OK) {
        monapi_warn("send failed\n", ret);
    }
    delete mi;
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
            intptr_t lastError;
            SharedMemory* mi = readFileAll(upperCase(msg.str).c_str(), lastError);
            if (NULL == mi) {
                monapi_warn("readFileAll(%s) error=%d", msg.str, lastError);
                Message::reply(&msg, lastError);
            } else {
                send_and_release_shm(mi, &msg);
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
            SharedMemory* memory = NULL;
            int ret = vmanager_.read(fileID, msg.arg2 /* size */, &memory);
            if (ret != M_OK) {
                Message::reply(&msg, ret);
            } else {
                ASSERT(memory);
                send_and_release_shm(memory, &msg);
            }
            break;
        }
        case MSG_FILE_WRITE:
        {
            uint32_t fileID = msg.arg1;
            SharedMemory* memory = new SharedMemory(msg.arg3, msg.arg2);
            ASSERT(memory->handle() != 0);
            // Use immediate map
            intptr_t result = memory->map(true);
            if (result != M_OK) {
                delete memory;
                Message::reply(&msg, result);
            } else {
                int ret = vmanager_.write(fileID, msg.arg2 /* size */, memory);
                delete memory;
                Message::reply(&msg, ret);
            }
            break;
        }
        case MSG_FILE_CLOSE:
        {
            int ret = vmanager_.close(msg.arg1);
            int status = Message::reply(&msg, ret);
            if (status != M_OK) {
                monapi_warn("monapi_file_close never return: error [%s]\n", monapi_error_string(status));
            }
            break;
        }
        case MSG_FILE_GET_SIZE:
        {
            Stat st;
            int ret = vmanager_.stat(msg.arg1, &st);
            Message::reply(&msg, ret, st.size);
            break;
        }
        case MSG_FILE_EXISTS:
        {
            bool fileExists = vmanager_.exists(upperCase(msg.str));
            Message::reply(&msg, fileExists);
            break;
        }
        case MSG_FILE_GET_SIZE_BY_PATH:
        {
            Stat st;
            int ret = vmanager_.stat(msg.str, &st);
            Message::reply(&msg, ret, st.size);
            break;
        }
        case MSG_FILE_GET_DATE:
        {
            Stat st;
            int ret = vmanager_.stat(msg.str, &st);
            KDate* p = (KDate*)msg.str;
            *p = st.datetime;
            Message::reply(&msg, ret, 0, msg.str);
            break;
        }
        case MSG_FILE_READ_DIRECTORY:
        {
            SharedMemory* memory;
            // quick hack
            int len = strlen(msg.str);
            if (len > 1 && msg.str[len - 1] == '/') {
                msg.str[len - 1] = 0;
            }
            int ret = vmanager_.read_directory(upperCase(msg.str).c_str(), &memory);
            if (ret != M_OK) {
                Message::reply(&msg, ret);
            } else {
                send_and_release_shm(memory, &msg);
            }
            break;
        }
        case MSG_FILE_DELETE:
        {
            int ret = vmanager_.delete_file(upperCase(msg.str).c_str());
            Message::reply(&msg, ret);
            break;
        }
        case MSG_FILE_DELETE_DIRECTORY:
        {
            int ret = vmanager_.delete_directory(upperCase(msg.str).c_str());
            Message::reply(&msg, ret);
            break;
        }
        case MSG_FILE_CREATE_DIRECTORY:
        {
            int ret = vmanager_.create_directory(upperCase(msg.str).c_str());
            Message::reply(&msg, ret);
            break;
        }
        case MSG_FILE_DECOMPRESS_ST5:
        {
            monapi_fatal("hige");
            // SharedMemory* mi1 = monapi_cmemoryinfo_new();
            // mi1->Handle = msg.arg1;
            // mi1->Size   = msg.arg2;
            // monapi_cmemoryinfo* mi2 = NULL;
            // if (monapi_cmemoryinfo_map(mi1, true) != M_OK) {
            //     mi2 = ST5Decompress(mi1);
            //     monapi_cmemoryinfo_dispose(mi1);
            // }
            // logprintf("MSG_FILE_DECOMPRESS_ST5 %d\n", mi2->Handle);
            // if (mi2 != NULL) {
            //     Message::reply(&msg, mi2->Handle, mi2->Size);
            //     monapi_cmemoryinfo_delete(mi2);
            // }
            // else
            // {
            //     Message::reply(&msg);
            // }
            // monapi_cmemoryinfo_delete(mi1);
            break;
        }
        case MSG_FILE_DECOMPRESS_ST5_FILE:
        {
            SharedMemory* mi = ST5DecompressFile(upperCase(msg.str).c_str());
            if (mi != NULL) {
                int ret = Message::sendReceive(&msg, msg.from, MSG_OK, msg.header, mi->handle(), mi->size());
                if (ret != M_OK) {
                    monapi_warn("send failed");
                }
                delete mi;
                // we can safely unmap it.
            } else {
                Message::reply(&msg);
            }
            break;
        }
        case MSG_STOP_SERVER:
            // end
            Message::reply(&msg, M_OK);
            return;
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

int64_t FileServer::GetST5DecompressedSize(SharedMemory* mi)
{
    int64_t ret = 0;
    ret = tek_checkformat(mi->size(), (unsigned char*)mi->data());
    return ret;
}

SharedMemory* FileServer::ST5Decompress(SharedMemory* mi)
{
    int64_t size = GetST5DecompressedSize(mi);
    if (size < 0) return NULL;

    // if size >= 4GB abort...
    if ((size >> 32) > 0) return NULL;

    SharedMemory *ret = new SharedMemory((uint32_t)(size + 1));
    if (ret->map(true) != M_OK) {
        delete ret;
        return NULL;
    }
//    ret->Size--;

    if (tek_decode(mi->size(), mi->data(), ret->data()) != 0) {
        // Decompress failed
        delete ret;
        return NULL;
    }

    ret->data()[ret->size() - 1] = 0;
    return ret;
}

SharedMemory* FileServer::ST5DecompressFile(const char* file)
{
    intptr_t lastError;
    SharedMemory* mi = readFileAll(file, lastError);
    SharedMemory* ret = NULL;
    if (mi == NULL) return ret;

    ret = ST5Decompress(mi);
    delete mi;
    return ret;
}
