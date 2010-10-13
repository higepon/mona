/*
 *   Copyright (c) 2010  Higepon(Taro Minowa)  <higepon@users.sourceforge.jp>
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef __FAT_H__
#define __FAT_H__

#include <stdlib.h>
#include <algorithm>
#include "IStorageDevice.h"
#include "vnode.h"
#include <string>
#include <vector>
#include <monapi/Buffer.h>

//  ToDo.
//      Check allowed charactor set on file creation.
//      Check duplicated name on same directory.
//
class StringUtil
{
public:
    static std::string reverseConcat(const std::vector<std::string>& strings)
    {
        std::string ret;
        for (std::vector<std::string>::const_reverse_iterator it = strings.rbegin(); it != strings.rend(); ++it) {
            ret += *it;
        }
        return ret;
    }

    static void rtrim(std::string& str)
    {
        std::string::size_type e = str.find_last_not_of(" \t\r\n");
        if(e == std::string::npos) {
            str.clear();
        } else {
            str.erase(e + 1);
        }
    }
};

typedef std::vector<uint8_t> Bytes;

// Long file name encoder
class LFNEncoder
{
public:
    LFNEncoder() {}
    ~LFNEncoder() {}

    // N.B.
    // For ascii only
    uint8_t* encode(std::string longName, uint32_t& size)
    {
        size = longName.size() * 2;
        uint8_t* buf = new uint8_t[size];
        ASSERT(buf);
        int index = 0;
        for (std::string::const_iterator it = longName.begin(); it != longName.end(); ++it) {
            buf[index++] = *it;
            buf[index++] = 0;
        }
        return buf;
    }
};

// Long file name decoder
class LFNDecoder
{
private:
    Bytes bytes_;
    Bytes::const_iterator it_;

    int getByte()
    {
        if (it_ == bytes_.end()) {
            return EOF;
        } else {
            int ret = (*it_);
            it_++;
            return ret;
        }
    }

    uint32_t utf16BytesToUTF32()
    {
        const int a = getByte();
        if (EOF == a) {
            return EOF;
        }
        const int b = getByte();
        if (EOF == b) {
            return '?';
        }

        if (a == 0 && b == 0) {
            return EOF;
        }
        const uint16_t val1 = ((b << 8) | a);
        if (val1 < 0xD800 || val1 > 0xDFFF) {
            return val1;
        }

        const int c = getByte();
        if (EOF == c) {
            return '?';
        }
        const int d = getByte();
        if (EOF == d) {
            return '?';
        }
        const uint16_t val2 = ((d << 8) | c);
        uint16_t hi = val1;
        uint16_t lo = val2;
        uint32_t X = (hi & ((1 << 6) -1)) << 10 | (lo & ((1 << 10) -1));
        uint32_t W = (hi >> 6) & ((1 << 5) - 1);
        uint32_t U = W + 1;
        uint32_t C = U << 16 | X;
        return C;
    }

public:
    LFNDecoder() {}
    ~LFNDecoder() {}

    void pushBytes(uint8_t* bytes, uint32_t length)
    {
        for (uint32_t i = 0; i < length; i++) {
            bytes_.push_back(bytes[i]);
        }
    }

    // For now supports only ascii
    std::string decode()
    {
        it_ = bytes_.begin();
        std::string ret;
        for (int c = utf16BytesToUTF32(); c != EOF; c = utf16BytesToUTF32()) {
            // ascii
            ret += (uint8_t)c;
        }
        bytes_.clear();
        return ret;
    }
};

class FatFileSystem : public FileSystem
{
public:
    FatFileSystem(IStorageDevice& dev) :
        dev_(dev),
        root_(new Vnode),
        fat_(NULL),
        buf_(NULL)
    {
        ASSERT(root_.get());

        if (!readBootParameters()) {
            monapi_fatal("can't read boot parameter block");
        }

        buf_ = new uint8_t[getClusterSizeByte()];
        ASSERT(buf_);

        if (!readFat()) {
            monapi_fatal("can't read file allocation table");
        }

        if (!readAndSetupRootDirectory()) {
            monapi_fatal("can't read root directory");
        }
    }

    ~FatFileSystem()
    {
        if (buf_ != NULL) {
            delete[] buf_;
        }
        if (fat_ != NULL) {
            delete[] fat_;
        }
    }

    uint32_t getClusterSizeByte() const
    {
        return getSectorsPerCluster() * SECTOR_SIZE;
    }

    uint32_t getFreeSize()
    {
        uint32_t numFreeClusters = 0;
        for (uint32_t i = 2; i < getSectorsPerFat() * SECTOR_SIZE / sizeof(uint32_t); i++) {
            if (fat_[i] == 0) {
                numFreeClusters++;
            }
        }
        return getClusterSizeByte() * numFreeClusters;
    }

    virtual int initialize()
    {
        return M_OK;
    }

    virtual int lookup(Vnode* directory, const std::string& file, Vnode** found, int type)
    {
        File* dir = getFileByVnode(directory);
        Files* childlen = dir->getChildlen();
        for (Files::const_iterator it = childlen->begin(); it != childlen->end(); ++it) {
            if (upperCase((*it)->getName()) == upperCase(file)) {
                if ((*it)->getVnode() == NULL) {
                    Vnode* newVnode = new Vnode;
                    ASSERT(newVnode);
                    newVnode->fnode = *it;
                    newVnode->type = type;
                    newVnode->fs = this;
                    (*it)->setVnode(newVnode);
                    *found = newVnode;
                } else {
                    *found = (*it)->getVnode();
                }
                return M_OK;
            }
        }
        return M_FILE_NOT_FOUND;
    }

    virtual int open(Vnode* vnode, intptr_t mode)
    {
        return M_OK;
    }
    virtual int read(Vnode* vnode, struct io::Context* context)
    {
        if (vnode->type != Vnode::REGULAR) {
            return M_FILE_NOT_FOUND;
        }
        File* e = getFileByVnode(vnode);
        uint32_t offset = context->offset;
        uint32_t sizeToRead = context->size;
        uint32_t rest = e->getSize() - offset;

        if (rest < sizeToRead) {
            sizeToRead = rest;
        }

        // Even for the case when sizeToRead == 0, we need context->memory.
        context->memory = monapi_cmemoryinfo_new();
        if (sizeToRead == 0) {
            return M_OK;
        }

        // Use immediate map for performance reason.
        if (monapi_cmemoryinfo_create(context->memory, sizeToRead, MONAPI_FALSE, true) != M_OK) {
            monapi_cmemoryinfo_delete(context->memory);
            return M_NO_SPACE;
        }
        uint32_t skipClusterCount = offset / getClusterSizeByte();
        uint32_t startCluster = getClusterAt(e, skipClusterCount);

        uint32_t sizeRead = 0;
        uint32_t offsetInBuf = context->offset - getClusterSizeByte() * skipClusterCount;

        MonAPI::Buffer readBuf(context->memory->Data, context->memory->Size);
        MonAPI::Buffer buf(buf_, getClusterSizeByte());

        for (uint32_t cluster = startCluster; ; cluster = fat_[cluster]) {
            ASSERT(!isEndOfCluster(cluster));
            if (!readCluster(cluster, buf_)) {
                return M_READ_ERROR;
            }
            uint32_t restSizeToRead = sizeToRead - sizeRead;
            uint32_t copySize = restSizeToRead > getClusterSizeByte() - offsetInBuf ? getClusterSizeByte() - offsetInBuf: restSizeToRead;
            ASSERT(context->memory->Data + sizeRead + copySize <= context->memory->Data + context->memory->Size);
            ASSERT(buf_ + offsetInBuf + copySize <= buf_ + getClusterSizeByte());
            bool isOk = MonAPI::Buffer::copy(readBuf, sizeRead, buf, offsetInBuf, copySize);
            ASSERT(isOk);
            sizeRead += copySize;
            offsetInBuf = 0;
            if (sizeRead == sizeToRead) {
                break;
            }
        }
        context->resultSize = sizeToRead;
        context->offset += sizeToRead;
        return M_OK;
    }

    virtual int write(Vnode* vnode, struct io::Context* context)
    {
        ASSERT(vnode->type == Vnode::REGULAR);
        ASSERT(context->memory);
        File* entry = getFileByVnode(vnode);
        uint32_t currentNumClusters = sizeToNumClusters(entry->getSize());
        int ret = expandFileAsNecessary(entry, context);
        if (ret != M_OK) {
            return ret;
        }
        uint32_t startClusterIndex = context->offset == 0 ? 0 : sizeToNumClusters(context->offset) - 1;
        uint32_t startCluster = getClusterAt(entry, startClusterIndex);
        ASSERT(!isEndOfCluster(startCluster));
        uint32_t sizeToWrite = context->size;
        uint32_t sizeWritten = 0;
        for (uint32_t cluster = startCluster, clusterIndex = startClusterIndex; !isEndOfCluster(cluster); clusterIndex++, cluster = fat_[cluster]) {
            uint32_t restSizeToWrite = sizeToWrite - sizeWritten;
            bool inNewCluster = clusterIndex >= currentNumClusters;
            if (!inNewCluster && !readCluster(cluster, buf_)) {
                    return M_READ_ERROR;
            }

            MonAPI::Buffer writeBuf(context->memory->Data, context->memory->Size);
            MonAPI::Buffer buf(buf_, getClusterSizeByte());

            uint32_t copySize;
            if (cluster == startCluster && context->offset != 0) {
                uint32_t offsetInCluster = context->offset % getClusterSizeByte();
                copySize = restSizeToWrite > getClusterSizeByte() - offsetInCluster ? getClusterSizeByte() - offsetInCluster: restSizeToWrite;
                bool isOk = MonAPI::Buffer::copy(buf, offsetInCluster, writeBuf, sizeWritten, copySize);
                ASSERT(isOk);
            } else {
                copySize = restSizeToWrite > getClusterSizeByte() ? getClusterSizeByte() : restSizeToWrite;
                bool isOK = MonAPI::Buffer::copy(buf, 0, writeBuf, sizeWritten, copySize);
                ASSERT(isOK);
            }
            sizeWritten += copySize;
            logprintf("%s %s:%d copySize=%d\n", __func__, __FILE__, __LINE__, copySize);
            if (!writeCluster(cluster, buf_)) {
                monapi_warn("write cluster failed cluster=%x\n", cluster);
                return M_WRITE_ERROR;
            }
        }

        ret = flushDirtyFat();
        if (ret != M_OK) {
            return ret;
        }
        context->offset += context->size;
        return context->size;
    }

    virtual int create(Vnode* dir, const std::string& file)
    {
        ASSERT(dir->type == Vnode::DIRECTORY);
        if (isLongName(file)) {
            intptr_t ret = createLongNameFile(dir, file);
            if (ret != M_OK) {
                return ret;
            } else {
                return M_OK;
            }
        } else {
            return createShortNameFile(dir, file);
        }
    }

    virtual int readdir(Vnode* directory, monapi_cmemoryinfo** entries)
    {
        typedef std::vector<monapi_directoryinfo> DirInfos;
        DirInfos dirInfos;
        ASSERT(directory->type == Vnode::DIRECTORY);
        File* dir = getFileByVnode(directory);
        for (Files::const_iterator it = dir->getChildlen()->begin(); it != dir->getChildlen()->end(); ++it) {
            monapi_directoryinfo di;
            strcpy(di.name, (*it)->getName().c_str());
            di.size = (*it)->getSize();
            di.attr = (*it)->isDirectory() ? ATTRIBUTE_DIRECTORY : 0;
            dirInfos.push_back(di);
        }
        monapi_cmemoryinfo* ret = monapi_cmemoryinfo_new();

        int size = dirInfos.size();
        if (monapi_cmemoryinfo_create(ret, sizeof(int) + size * sizeof(monapi_directoryinfo), MONAPI_FALSE, true) != M_OK) {
            monapi_cmemoryinfo_delete(ret);
            return M_NO_MEMORY;
        }

        MonAPI::Buffer dest(ret->Data, ret->Size);
        MonAPI::Buffer src(&size, sizeof(int));
        bool isOK = MonAPI::Buffer::copy(dest, src, sizeof(int));
        ASSERT(isOK);
        monapi_directoryinfo* p = (monapi_directoryinfo*)&ret->Data[sizeof(int)];
        *entries = ret;
        for (DirInfos::const_iterator it = dirInfos.begin(); it != dirInfos.end(); ++it) {
            ASSERT((uintptr_t)p < (uintptr_t)(ret->Data + ret->Size));
            *p = *it;
            p++;
        }
        return M_OK;
    }

    virtual int close(Vnode* vnode)
    {
        return M_OK;
    }

    virtual int truncate(Vnode* vnode)
    {
        ASSERT(vnode->type == Vnode::REGULAR);
        File* entry = getFileByVnode(vnode);
        if (entry->getSize() == 0) {
            return M_OK;
        }
        if (!readCluster(entry->getClusterInParent(), buf_)) {
            return M_READ_ERROR;
        }
        struct de* theEntry = ((struct de*)buf_) + entry->getIndexInParentCluster();
        setEntry(theEntry, 0, 0);

        logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        if (!writeCluster(entry->getClusterInParent(), buf_)) {
            return M_WRITE_ERROR;
        }

        int ret = freeClusters(entry);
        if (ret != M_OK) {
            return ret;
        }
        entry->setStartCluster(0);
        entry->setSize(0);
        return M_OK;
    }

    virtual int delete_file(Vnode* vnode)
    {
        if(vnode == root_) {
            return M_BAD_ARG; // root is undeletable
        }
        File* entry = getFileByVnode(vnode);
        if (!readCluster(entry->getClusterInParent(), buf_)) {
            return M_READ_ERROR;
        }
        struct de* theEntry = ((struct de*)buf_) + entry->getIndexInParentCluster();
        theEntry->name[0] = FREE_ENTRY;
        logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        if (!writeCluster(entry->getClusterInParent(), buf_)) {
            return M_WRITE_ERROR;
        }
        int ret = freeClusters(entry);
        if (ret != M_OK) {
            return ret;
        }
        entry->getParent()->removeChild(entry);
        destroyVnode(vnode);
        return M_OK;
    }

    virtual int stat(Vnode* vnode, Stat* st)
    {
        File* entry = getFileByVnode(vnode);
        st->size = entry->getSize();
        return M_OK;
    }

    virtual void destroyVnode(Vnode* vnode)
    {
        File* entry = getFileByVnode(vnode);
        delete entry;
        delete vnode;
    }

    Vnode* getRoot() const
    {
        return root_.get();
    }

    // Public for testability
    uint16_t getBytesPerSector() const
    {
        return little2host16(bsbpb_->bps);
    }

    class File;
    typedef std::vector<File*> Files;

    // Public for testability
    class File
    {
    public:
        File(const std::string& name, uintptr_t size, uint32_t startCluster, uint32_t clusterInParent, uint32_t indexInParentCluster) :
            name_(name),
            size_(size),
            startCluster_(startCluster),
            parent_(NULL),
            clusterInParent_(clusterInParent),
            indexInParentCluster_(indexInParentCluster),
            childlen_(NULL),
            isDirectory_(false),
            vnode_(NULL)
        {
        }

        File(const std::string& name, uintptr_t startCluster, const Files& childlen, uint32_t clusterInParent, uint32_t indexInParentCluster) :
            name_(name),
            startCluster_(startCluster),
            parent_(NULL),
            clusterInParent_(clusterInParent),
            indexInParentCluster_(indexInParentCluster),
            childlen_(new Files()),
            isDirectory_(true),
            vnode_(NULL)
        {
            childlen_->resize(childlen.size());
            std::copy(childlen.begin(), childlen.end(), childlen_->begin());
        }

        virtual ~File()
        {
            if (isDirectory()) {
                for (Files::const_iterator it = childlen_->begin(); it != childlen_->end(); ++it) {
                    delete *it;
                }
            }
        }

        bool isDirectory() const
        {
            return isDirectory_;
        }

        void setVnode(Vnode* vnode)
        {
            vnode_ = vnode;
        }

        Vnode* getVnode() const
        {
            return vnode_;
        }

        const std::string& getName() const
        {
            return name_;
        }

        uintptr_t getSize() const
        {
            return size_;
        }

        void setSize(uintptr_t size)
        {
            size_ = size;
        }

        void setStartCluster(uintptr_t cluster)
        {
            startCluster_ = cluster;
        }

        uintptr_t getStartCluster() const
        {
            return startCluster_;
        }

        File* getParent() const
        {
            return parent_;
        }

        void setParent(File* parent)
        {
            parent_ = parent;
        }

        uint32_t getClusterInParent() const
        {
            return clusterInParent_;
        }

        uint32_t getIndexInParentCluster() const
        {
            return indexInParentCluster_;
        }
        void addChild(File* entry)
        {
            ASSERT(isDirectory());
            childlen_->push_back(entry);
        }

        Files* getChildlen()
        {
            return childlen_.get();
        }

        void removeChild(File* entry)
        {
            ASSERT(isDirectory());
            childlen_->erase(std::remove(childlen_->begin(), childlen_->end(), entry), childlen_->end());
        }

    private:
        const std::string name_;
        uintptr_t size_;
        uintptr_t startCluster_;
        File* parent_;
        uint32_t clusterInParent_;
        uint32_t indexInParentCluster_;
        MonAPI::scoped_ptr<Files> childlen_;
        bool isDirectory_;
        Vnode* vnode_;
    };

private:
    typedef std::vector<uint32_t> Clusters;

    bool isEndOfCluster(uint32_t cluster) const
    {
        return cluster >= END_OF_CLUSTER;
    }

    bool allocateClusters(Clusters& clusters, uint32_t numClusters)
    {
        for (uint32_t i = 2; i < getSectorsPerFat() * SECTOR_SIZE / sizeof(uint32_t); i++) {
            if (numClusters == 0) {
                return true;
            }
            if (fat_[i] == 0) {
                fat_[i] = END_OF_CLUSTER;
                clusters.push_back(i);
                numClusters--;
            }
        }
        for (Clusters::iterator it = clusters.begin(); it != clusters.end(); ++it) {
            fat_[*it] = 0;
        }
        clusters.clear();
        return false;
    }

    uint32_t allocateCluster()
    {
        // 0th, 1st fat entry is reserved.
        for (uint32_t i = 2; i < getSectorsPerFat() * SECTOR_SIZE / sizeof(uint32_t); i++) {
            if (fat_[i] == 0) {
                fat_[i] = END_OF_CLUSTER;
                return i;
            }
        }
        return END_OF_CLUSTER;
    }

    uint32_t getClusterAt(File* entry, uint32_t index)
    {
        uint32_t cluster = entry->getStartCluster();
        for (uint32_t i = 0; i < index; i++, cluster = fat_[cluster]) {
            ASSERT(!isEndOfCluster(cluster));
        }
        return cluster;
    }

    uint32_t getLastClusterByVnode(Vnode* vnode)
    {
        return getLastCluster(getFileByVnode(vnode));
    }

    uint32_t getLastCluster(File* entry)
    {
        for (uint32_t cluster = entry->getStartCluster(); ; cluster = fat_[cluster]) {
            if (isEndOfCluster(fat_[cluster])) {
                return cluster;
            }
        }
    }

    bool readBootParameters()
    {
        if (dev_.read(0, bootParameters_, SECTOR_SIZE) != M_OK) {
            return false;
        }
        bsbpb_ = (struct bsbpb*)(bootParameters_ + sizeof(struct bs));
        bsxbpb_ = (struct bsxbpb*)(bootParameters_ + sizeof(struct bs) + sizeof(struct bsbpb));

        ASSERT(getBytesPerSector() == 512);
        ASSERT(getSectorsPerCluster() <= 64); // Max 32KB
        ASSERT(getNumberOfFats() == 2);
        return true;
    }

    bool readFat()
    {
        uint32_t fatStartSector = getReservedSectors();
        uint32_t fatSizeByte = getSectorsPerFat() * SECTOR_SIZE;
        fat_ = new uint32_t[fatSizeByte / sizeof(uint32_t)];
        ASSERT(fat_);

        if (dev_.read(fatStartSector, fat_, fatSizeByte) != M_OK) {
            return false;
        }
        return true;
    }

    bool readCluster(uint32_t cluster, uint8_t* buf)
    {
        uint32_t firstDataSector = getReservedSectors() + getNumberOfFats() * getSectorsPerFat();
        uint32_t absoluteCluster = firstDataSector / getSectorsPerCluster() + cluster - 2;
        if (dev_.read(absoluteCluster * getSectorsPerCluster(), buf, SECTOR_SIZE * getSectorsPerCluster()) != M_OK) {
            return false;
        }
        return true;
    }

    bool writeCluster(uint32_t cluster, const uint8_t* buf)
    {
        uint32_t firstDataSector = getReservedSectors() + getNumberOfFats() * getSectorsPerFat();
        uint32_t absoluteCluster = firstDataSector / getSectorsPerCluster() + cluster - 2;
        logprintf("write cluster %d\n", cluster);
        if (dev_.write(absoluteCluster * getSectorsPerCluster(), buf, SECTOR_SIZE * getSectorsPerCluster()) != M_OK) {
            return false;
        }
        return true;
    }

    bool readDirectory(uint32_t startCluster, Files& childlen)
    {
        MonAPI::scoped_ptr<uint8_t> buf(new uint8_t[getClusterSizeByte()]);
        ASSERT(buf.get());
        LFNDecoder decoder;
        for (uint32_t cluster = startCluster; !isEndOfCluster(cluster); cluster = fat_[cluster]) {
            if (!readCluster(cluster, buf.get()) != M_OK) {
                return false;
            }
            bool hasLongName = false;
            std::vector<std::string> partialLongNames;
            uint32_t index = 0;
            for (struct de* entry = (struct de*)(buf.get()); (uint8_t*)entry < (uint8_t*)(&((buf.get()))[getClusterSizeByte()]); entry++, index++) {
                if (entry->name[0] == AVAILABLE_ENTRY || entry->name[0] == DOT_ENTRY || entry->name[0] == FREE_ENTRY) {
                    continue;
                }
                // Long file name
                if (entry->attr == ATTR_LFN) {
                    struct lfn* lfn = (struct lfn*)entry;
                    decoder.pushBytes(lfn->name1, sizeof(lfn->name1));
                    decoder.pushBytes(lfn->name2, sizeof(lfn->name2));
                    decoder.pushBytes(lfn->name3, sizeof(lfn->name3));
                    partialLongNames.push_back(decoder.decode());
                    if (lfn->seq == 1) {
                        hasLongName = true;
                    }
                    continue;
                }
                std::string filename;
                if (hasLongName) {
                    filename = StringUtil::reverseConcat(partialLongNames);
                } else {
                    filename = std::string((char*)entry->name, 8);
                    StringUtil::rtrim(filename);
                    std::string ext = std::string((char*)entry->ext, 3);
                    StringUtil::rtrim(ext);
                    if (!ext.empty()) {
                        filename += '.';
                        filename += ext;
                    }
                }
                File* target = NULL;
                if (entry->attr & ATTR_SUBDIR) {
                    Files subChildlen;
                    // For now, we read all directories recursively.
                    // This may cause slower initialization.
                    if (!readDirectory(little2host16(entry->clus), subChildlen)) {
                        return false;
                    }
                    target = new File(filename, little2host16(entry->clus), subChildlen, cluster, index);
                } else {
                    target = new File(filename, little2host32(entry->size), little2host16(entry->clus), cluster, index);
                }
                ASSERT(target);
                childlen.push_back(target);
                partialLongNames.clear();
            }
        }
        return true;
    }

    bool readAndSetupRootDirectory()
    {
        Files childlen;
        if (!readDirectory(getRootDirectoryCluster(), childlen)) {
            return false;
        }

        root_->fnode = new File("", getRootDirectoryCluster(), childlen, 0, 0);
        ASSERT(root_->fnode);
        root_->fs = this;
        root_->type = Vnode::DIRECTORY;

        for (Files::iterator it = childlen.begin(); it != childlen.end(); ++it) {
            (*it)->setParent(getFileByVnode(root_.get()));
        }

        return true;
    }

    uint8_t getSectorsPerCluster() const
    {
        return bsbpb_->spc;
    }

    uint8_t getNumberOfFats() const
    {
        return bsbpb_->nft;
    }

    uint16_t getFatVersion() const
    {
        return little2host16(bsxbpb_->vers);
    }

    uint16_t getRootDirectoryCluster() const
    {
        return little2host32(bsxbpb_->rdcl);
    }

    uint16_t getReservedSectors() const
    {
        return little2host16(bsbpb_->res);
    }

    uint32_t getSectorsPerFat() const
    {
        // For Fat32, small spf should be zero.
        ASSERT(little2host16(bsbpb_->spf) == 0);
        return little2host32(bsxbpb_->bspf);
    }

    uint16_t little2host16(uint8_t* p) const
    {
        return (p[1] << 8) | (p[0]);
    }

    uint32_t little2host32(uint8_t* p) const
    {
        return ((p[3] << 24) | (p[2] << 16) | (p[1] << 8) | p[0]);
    }

    std::vector<std::string> split(std::string str, char delimiter)
    {
        std::vector<std::string> ret;
        std::string::size_type index;

        while (true) {
            index = str.find(delimiter);
            if (index == std::string::npos) {
                ret.push_back(str);
                break;
            }
            else {
                ret.push_back(str.substr(0, index));
                str = str.substr(++index);
            }
        }

        return ret;
    }


    enum
    {
        AVAILABLE_ENTRY = 0x00,
        DOT_ENTRY = 0x2e,
        FREE_ENTRY = 0xe5,
        SECTOR_SIZE = 512,
        ATTR_SUBDIR = 0x10,
        ATTR_LFN = 0x0f,
        LAST_LFN_ENTRY = 0x40,
        END_OF_CLUSTER = 0x0FFFFFF8,
        LFN_NAME_LEN_PER_ENTRY = 13,
        MAX_LONG_NAME = 255,
        forbiddend_comma
    };

    struct bs {
        uint8_t jmp[3];            /* bootstrap entry point */
        uint8_t oem[8];            /* OEM name and version */
    };

    // BPB (BIOS Parameter Block)
    struct bsbpb {
        uint8_t bps[2];            /* bytes per sector */
        uint8_t spc;               /* sectors per cluster */
        uint8_t res[2];            /* reserved sectors */
        uint8_t nft;               /* number of FATs */
        uint8_t rde[2];            /* root directory entries */
        uint8_t sec[2];            /* total sectors */
        uint8_t mid;               /* media descriptor */
        uint8_t spf[2];            /* sectors per FAT */
        uint8_t spt[2];            /* sectors per track */
        uint8_t hds[2];            /* drive heads */
        uint8_t hid[4];            /* hidden sectors */
        uint8_t bsec[4];           /* big total sectors */
    };

    // Extended Boot Record
    struct bsxbpb {
        uint8_t bspf[4];           /* big sectors per FAT */
        uint8_t xflg[2];           /* FAT control flags */
        uint8_t vers[2];           /* file system version */
        uint8_t rdcl[4];           /* root directory start cluster */
        uint8_t infs[2];           /* file system info sector */
        uint8_t bkbs[2];           /* backup boot sector */
        uint8_t rsvd[12];          /* reserved */
    };

    struct de {
        uint8_t name[8];           /* name */
        uint8_t ext[3];            /* extension */
        uint8_t attr;              /* attributes */
        uint8_t rsvd[10];          /* reserved */
        uint8_t time[2];           /* creation time */
        uint8_t date[2];           /* creation date */
        uint8_t clus[2];           /* starting cluster */
        uint8_t size[4];           /* size */
    };

    struct lfn {
        uint8_t seq;               /* seq */
        uint8_t name1[10];         /* name1 */
        uint8_t attr;              /* attributes */
        uint8_t rsvd;              /* reserved */
        uint8_t chksum;            /* check sum */
        uint8_t name2[12];         /* name2 */
        uint8_t clus[2];           /* starting cluster */
        uint8_t name3[4];           /* name3 */
    };

    uint16_t packDate(uint16_t year, uint8_t month, uint8_t day)
    {
        return ((year - 1980) << 9) | (month << 5) | day;
    }

    uint16_t packTime(uint8_t hours, uint8_t minitues, uint8_t seconds)
    {
        // todo : handle JST here?
        return ((hours + 9) << 11) | (minitues << 5) | (seconds / 2);
    }

    void setEntry(struct de* entry, uint32_t cluster, uint32_t size)
    {
        *((uint32_t*)entry->size) = size;
        *((uint16_t*)entry->clus) = cluster;
        MonAPI::Date date;
        *((uint16_t*)entry->date) = packDate(date.year(), date.month(), date.day());
        *((uint16_t*)entry->time) = packTime(date.hour(), date.min(), date.sec());
    }

    void initializeEntry(struct de* entry, const std::string& name, const std::string& ext)
    {
        ASSERT(name.size() <= 8);
        ASSERT(ext.size() <= 3);
        memset(entry->name, ' ', 8);
        memcpy(entry->name, name.c_str(), name.size());
        memset(entry->ext, ' ', 3);
        memcpy(entry->ext, ext.c_str(), ext.size());
        setEntry(entry, 0, 0);
    }

    void initializeEntry(struct de* entry, const std::string& filename)
    {
        std::vector<std::string> nameAndExt = split(filename, '.');
        ASSERT(nameAndExt.size() == 2);
        initializeEntry(entry, nameAndExt[0], nameAndExt[1]);
    }

    intptr_t updateParentCluster(File* entry)
    {
        if (!readCluster(entry->getClusterInParent(), buf_)) {
            return M_READ_ERROR;
        }

        struct de* theEntry = ((struct de*)buf_) + entry->getIndexInParentCluster();
        setEntry(theEntry, entry->getStartCluster(), entry->getSize());
        logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        if (!writeCluster(entry->getClusterInParent(), buf_)) {
            return M_WRITE_ERROR;
        }
        return M_OK;
    }

    int expandFile(uint32_t lastCluster, uint32_t newCluster)
    {
        updateFatNoFlush(lastCluster, newCluster);
        updateFatNoFlush(newCluster, END_OF_CLUSTER);
        int ret = flushDirtyFat();
        if (ret != M_OK) {
            return ret;
        }
        return M_OK;
    }

    int expandFileAsNecessary(File* entry, io::Context* context)
    {
        uint32_t tailOffset = context->offset + context->size;
        uint32_t currentNumClusters = sizeToNumClusters(entry->getSize());
        uint32_t newNumClusters = tailOffset > entry->getSize() ? sizeToNumClusters(tailOffset) : currentNumClusters;

        if (newNumClusters == currentNumClusters) {
            if (tailOffset > entry->getSize()) {
                entry->setSize(tailOffset);
            }
            if (updateParentCluster(entry) != M_OK) {
                return M_WRITE_ERROR;
            }
            return M_OK;
        }
        Clusters clusters;
        if (!allocateClusters(clusters, newNumClusters - currentNumClusters)) {
            return M_NO_SPACE;
        }
        ASSERT(clusters.size() > 0);
        uint32_t orgSize = entry->getSize();

        bool isParentClusterDirty = false;
        if (tailOffset > entry->getSize()) {
            entry->setSize(tailOffset);
            isParentClusterDirty = true;
        }
        if (orgSize == 0) {
            entry->setStartCluster(clusters[0]);
            isParentClusterDirty = true;
        } else {
            updateFatNoFlush(getLastCluster(entry), clusters[0]);
        }
        for (uint32_t i = 0; i < clusters.size() - 1; i++) {
            updateFatNoFlush(clusters[i], clusters[i + 1]);
        }
        updateFatNoFlush(clusters[clusters.size() - 1], END_OF_CLUSTER);

        if (isParentClusterDirty) {
            if (updateParentCluster(entry) != M_OK) {
                return M_WRITE_ERROR;
            }
        }
        return M_OK;
    }

    int freeClusters(File* entry)
    {
        for (uint32_t cluster = entry->getStartCluster(); !isEndOfCluster(cluster);) {
            uint32_t nextCluster = fat_[cluster];
            updateFatNoFlush(cluster, 0);
            cluster = nextCluster;
        }
        int ret = flushDirtyFat();
        if (ret != M_OK) {
            return ret;
        }
        return M_OK;
    }

    int allocateContigousEntries(struct de* entries, uint32_t numEntries)
    {
        std::vector<uint32_t> foundEntryIndexes;
        for (uint32_t i = 0; i < getClusterSizeByte() / sizeof(struct de); i++) {
            if (entries[i].name[0] == AVAILABLE_ENTRY || entries[i].name[0] == FREE_ENTRY) {
                if (!foundEntryIndexes.empty() && foundEntryIndexes[foundEntryIndexes.size() - 1] != i - 1) {
                    foundEntryIndexes.clear();
                }
                foundEntryIndexes.push_back(i);
                if (foundEntryIndexes.size() == numEntries) {
                    return foundEntryIndexes[0];
                }
            }
        }
        return M_NO_SPACE;
    }

    File* getFileByVnode(Vnode* vnode)
    {
        return (File*)(vnode->fnode);
    }

    uint32_t sizeToNumClusters(uint32_t sizeByte) const
    {
        if (sizeByte == 0) {
            return 0;
        } else {
            return (sizeByte + getClusterSizeByte() - 1) / getClusterSizeByte();
        }
    }
    uint8_t checksum(const std::string& name, const std::string& ext)
    {
        // name and ext should be rpad with space.
        ASSERT(name.size() == 8);
        ASSERT(ext.size() == 3);
        std::string file = name + ext;
        unsigned char sum = 0;
        for (std::string::const_iterator it = file.begin(); it != file.end(); ++it) {
            sum = ((sum & 1) << 7) + (sum >> 1) + *it;
        }
        return sum;
    }

    void createAndAddFile(Vnode* dir, const std::string& name, uint32_t clusterInParent, uint32_t indexInParentCluster)
    {
        File* file = new File(name, 0, 0, clusterInParent, indexInParentCluster);
        ASSERT(file);
        File* d = getFileByVnode(dir);
        d->addChild(file);
        file->setParent(d);
    }

    int tryCreateNewEntryInCluster(Vnode* dir, const std::string&file, uint32_t cluster)
    {
        if (!readCluster(cluster, buf_)) {
            return M_READ_ERROR;
        }

        const int ENTRIES_PER_CLUSTER = getClusterSizeByte() / sizeof(struct de);
        struct de* entries = (struct de*)buf_;
        for (int i = 0; i < ENTRIES_PER_CLUSTER; i++) {
            if (entries[i].name[0] == AVAILABLE_ENTRY || entries[i].name[0] == FREE_ENTRY) {
                createAndAddFile(dir, file, cluster, i);
                initializeEntry(&entries[i], file);
                logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
                if (writeCluster(cluster, buf_)) {
                    return M_OK;
                } else {
                    return M_WRITE_ERROR;
                }
            }
        }
        return M_NO_SPACE;
    }

    int flushFat(uint32_t cluster)
    {
        uint32_t fatStartSector = getReservedSectors();
        uint32_t dirtyFatSector = (cluster * sizeof(uint32_t)) / SECTOR_SIZE + fatStartSector;
        uint8_t* dirtyFat = (uint8_t*)fat_ + ((cluster * sizeof(uint32_t)) / SECTOR_SIZE) * SECTOR_SIZE;
        logprintf("flushFat %d\n", dirtyFatSector);
        int ret = dev_.write(dirtyFatSector, dirtyFat, SECTOR_SIZE);
        if (ret != M_OK) {
            monapi_warn("failed to update FAT");
            return ret;
        } else {
            return M_OK;
        }
    }

    int flushDirtyFat()
    {
        for (std::map<uint32_t, uint32_t>::const_iterator it = dirtyFat_.begin(); it != dirtyFat_.end(); ++it) {

            int ret = flushFat(it->first);
            if (ret != M_OK) {
                dirtyFat_.clear();
                return ret;
            }
        }
        dirtyFat_.clear();
        return M_OK;
    }

    void updateFatNoFlush(uint32_t index, uint32_t cluster)
    {
        fat_[index] = cluster;
        dirtyFat_.insert(std::pair<uint32_t, uint32_t>(index, index));
    }

    bool isLongName(const std::string name) const
    {
        return name.size() > 11;
    }

    void copy(uint8_t* dest, int destLen, uint8_t* source, int size, int sourceOffset, int sourceLen)
    {
        int restSize = sourceLen - sourceOffset;
        int copySize = restSize < size ? restSize : size;
        if (copySize > 0) {
            MonAPI::Buffer d(dest, destLen);
            MonAPI::Buffer s(source, sourceLen);
            bool isOK = MonAPI::Buffer::copy(d, 0, s, sourceOffset, copySize);
            ASSERT(isOK);
        }
    }

    int createLongNameEntryInCluster(Vnode* dir, uint8_t* buf, uint32_t targetCluster, const std::string& file, uint32_t startIndex, uint32_t numEntries)
    {
        LFNEncoder encoder;
        uint32_t encodedLen;
        const char* DUMMY_SHORT_NAME = "SHORT   ";
        const char* DUMMY_SHORT_EXT = "TXT";
        MonAPI::scoped_ptr<uint8_t> encodedName(encoder.encode(file, encodedLen));

        for (uint32_t i = startIndex, seq = numEntries - 1; ; i++) {
            if (seq == 0) {
                struct de* entry = (struct de*)buf + i;
                initializeEntry(entry, DUMMY_SHORT_NAME, DUMMY_SHORT_EXT);
                createAndAddFile(dir, file, targetCluster, i);
                break;
            } else {
                struct lfn* p = (struct lfn*)(buf) + i;
                memset(p, 0, sizeof(struct lfn));
                p->attr = ATTR_LFN;
                p->chksum = checksum(DUMMY_SHORT_NAME, DUMMY_SHORT_EXT);
                const int NAME_BYTES_PER_ENTRY = LFN_NAME_LEN_PER_ENTRY * 2;

                int sourceOffset = (seq - 1) * NAME_BYTES_PER_ENTRY;
                copy(p->name1, sizeof(p->name1), encodedName.get(), sizeof(p->name1), sourceOffset, encodedLen);
                copy(p->name2, sizeof(p->name2), encodedName.get(), sizeof(p->name2), sourceOffset+ sizeof(p->name1), encodedLen);
                copy(p->name3, sizeof(p->name3), encodedName.get(), sizeof(p->name3), sourceOffset+ sizeof(p->name1) + sizeof(p->name2), encodedLen);
                p->seq = seq--;
                if (i == startIndex) {
                    // The last LFN entry comes first in the cluster
                    p->seq |= LAST_LFN_ENTRY;
                }
            }
        }
        logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        if (!writeCluster(targetCluster, buf)) {
            return M_WRITE_ERROR;
        }
        return M_OK;
    }

    int createLongNameFile(Vnode* dir, const std::string& file)
    {
        if (file.size() > MAX_LONG_NAME) {
            return M_NO_SPACE;
        }
        const int ENTRIES_PER_CLUSTER = getClusterSizeByte() / sizeof(struct de);
        uint32_t requiredNumEntries = (file.size() + LFN_NAME_LEN_PER_ENTRY - 1) / LFN_NAME_LEN_PER_ENTRY + 1;

        // It seems that vfat on Linux doesn't allow lfn entries clusterred into multiple clusters.
        // But I'm not sure :D.
        if (requiredNumEntries > (uint32_t)ENTRIES_PER_CLUSTER) {
            return M_NO_SPACE;
        }
        uint32_t lastCluster = getLastClusterByVnode(dir);
        if (!readCluster(lastCluster, buf_)) {
            return M_READ_ERROR;
        }

        uint32_t extraCluster = END_OF_CLUSTER;
        int startIndex = 0;
        if ((startIndex = allocateContigousEntries((struct de*)buf_, requiredNumEntries)) < 0) {
            ASSERT(startIndex == M_NO_SPACE);
            extraCluster = allocateCluster();
            if (isEndOfCluster(extraCluster)) {
                return M_NO_SPACE;
            }
            int ret = expandFile(lastCluster, extraCluster);
            if (ret != M_OK) {
                return ret;
            }
        }

        bool inExtraCluster = !isEndOfCluster(extraCluster);
        if (inExtraCluster) {
            memset(buf_, 0, getClusterSizeByte());
        }
        uint32_t targetCluster = inExtraCluster ? extraCluster : lastCluster;
        startIndex = inExtraCluster ? 0 :startIndex;
        return createLongNameEntryInCluster(dir, buf_, targetCluster, file, startIndex, requiredNumEntries);
    }

    int createShortNameFile(Vnode* dir, const std::string& file)
    {
        uint32_t cluster = getLastClusterByVnode(dir);
        int ret = tryCreateNewEntryInCluster(dir, file, cluster);
        if (ret == M_OK) {
            return M_OK;
        } else if (ret == M_NO_SPACE) {
            uint32_t newCluster = allocateCluster();
            if (isEndOfCluster(newCluster)) {
                return M_NO_SPACE;
            }

            int ret = tryCreateNewEntryInCluster(dir, file, newCluster);
            if (ret != M_OK) {
                return ret;
            }

            updateFatNoFlush(cluster, newCluster);
            updateFatNoFlush(newCluster, END_OF_CLUSTER);
            ret = flushDirtyFat();
            if (ret != M_OK) {
                return ret;
            }
            return M_OK;
        } else {
            return ret;
        }
    }

    std::string upperCase(const std::string& s)
    {
        std::string result = s;
        std::transform(result.begin(), result.end(), result.begin(), toupper);
        return result;
    }


    uint8_t bootParameters_[SECTOR_SIZE];
    struct bsbpb* bsbpb_;
    struct bsxbpb* bsxbpb_;
    IStorageDevice& dev_;
    MonAPI::scoped_ptr<Vnode> root_;
    uint32_t* fat_;
    std::map<uint32_t, uint32_t> dirtyFat_;
    uint8_t* buf_;
};

#endif // __FAT_H__
