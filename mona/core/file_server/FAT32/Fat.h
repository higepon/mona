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
#include "VnodeManager.h"

// todo
//  trim
//  long file name
// get

// host2 little
// create duplicate check

// merge directory and entry.
// java api isDirectory.

// write
// delete_file delete from cacher
// hige.txt is seen on root?
// check all warnings

// fat class
class FatFileSystem : public FileSystem
{
public:

    uint32_t getFreeSize()
    {
        uint32_t numFreeClusters = 0;
        for (uint32_t i = 2; i < getSectorsPerFat() * SECTOR_SIZE / sizeof(uint32_t); i++) {
            if (fat_[i] == 0) {
                numFreeClusters++;
            }
        }
        logprintf("numFreeClusters=%d", numFreeClusters);
        return getClusterSizeByte() * numFreeClusters;
    }

    virtual int initialize()
    {
        return MONA_SUCCESS;
    }
    virtual int lookup(Vnode* diretory, const std::string& file, Vnode** found, int type)
    {
        Vnode* v = vnodeManager_.cacher()->lookup(diretory, file);
        if (v != NULL && v->type == type) {
            *found = v;
            return MONA_SUCCESS;
        }

        if (diretory->type != Vnode::DIRECTORY) {
            return MONA_ERROR_ENTRY_NOT_FOUND;
        }

        Directory* dir = (Directory*)diretory->fnode;
        Files& childlen = dir->getChildlen();
        for (Files::const_iterator it = childlen.begin(); it != childlen.end(); ++it) {
            logprintf("getName=%s file = %s\n", (*it)->getName().c_str(), file.c_str());
            if ((*it)->getName() == file) {
                Vnode* newVnode = vnodeManager_.alloc();
                ASSERT(newVnode);
                newVnode->fnode = *it;
                newVnode->type = type;
                newVnode->fs = this;
                vnodeManager_.cacher()->add(diretory, file, newVnode);
                *found = newVnode;
                return MONA_SUCCESS;
            }
        }
        return MONA_ERROR_ENTRY_NOT_FOUND;
    }

    virtual int open(Vnode* vnode, intptr_t mode)
    {
        return MONA_SUCCESS;
    }
    virtual int read(Vnode* vnode, struct io::Context* context)
    {
        if (vnode->type != Vnode::REGULAR) {
            return MONA_FAILURE;
        }
        File* e = (File*)vnode->fnode;
        uint32_t offset = context->offset;
        uint32_t sizeToRead = context->size;
        uint32_t rest = e->getSize() - offset;

        if (rest < sizeToRead) {
            sizeToRead = rest;
        }

        if (sizeToRead == 0) {
            return MONA_SUCCESS;
        }

        context->memory = monapi_cmemoryinfo_new();
        // Use immediate map for performance reason.
        if (monapi_cmemoryinfo_create(context->memory, sizeToRead, MONAPI_FALSE, true) != M_OK) {
            monapi_cmemoryinfo_delete(context->memory);
            return MONA_ERROR_MEMORY_NOT_ENOUGH;
        }

        uint32_t skipClusterCount = offset / getClusterSizeByte();
        uint32_t startCluster = getClusterAt(e, skipClusterCount);

        uint8_t buf[getClusterSizeByte()];
        uint32_t sizeRead = 0;
        uint32_t offsetInBuf = context->offset - getClusterSizeByte() * skipClusterCount;
        for (uint32_t cluster = startCluster; ; cluster = fat_[cluster]) {
            ASSERT(!isEndOfCluster(cluster));
            if (!readCluster(cluster, buf)) {
                return MONA_FAILURE;
            }
            uint32_t restSizeToRead = sizeToRead - sizeRead;
            uint32_t copySize = restSizeToRead > getClusterSizeByte() - offsetInBuf ? getClusterSizeByte() - offsetInBuf: restSizeToRead;
            memcpy(context->memory->Data + sizeRead, buf + offsetInBuf , copySize);
            sizeRead += copySize;
            offsetInBuf = 0;
            if (sizeRead == sizeToRead) {
                break;
            }
        }
        context->resultSize = sizeToRead;
        context->offset += sizeToRead;
        return MONA_SUCCESS;
    }

    uint32_t sizeToNumClusters(uint32_t sizeByte) const
    {
        if (sizeByte == 0) {
            return 1;
        } else {
            return (sizeByte + getClusterSizeByte() - 1) / getClusterSizeByte();
        }
    }

    virtual int write(Vnode* vnode, struct io::Context* context)
    {
        ASSERT(vnode->type == Vnode::REGULAR);
        ASSERT(context->memory);
        File* entry = (File*)vnode->fnode;
        uint32_t currentNumClusters = sizeToNumClusters(entry->getSize());
        if (expandFileAsNecessary(entry, context) != M_OK) {
            return -1;
        }
        uint32_t startClusterIndex = sizeToNumClusters(context->offset) - 1;
        uint32_t startCluster = getClusterAt(entry, startClusterIndex);
        ASSERT(!isEndOfCluster(startCluster));
        uint32_t sizeToWrite = context->size;
        uint32_t sizeWritten = 0;
        for (uint32_t cluster = startCluster, clusterIndex = startClusterIndex; !isEndOfCluster(cluster); clusterIndex++, cluster = fat_[cluster]) {
            uint32_t restSizeToWrite = sizeToWrite - sizeWritten;
            bool inNewCluster = clusterIndex >= currentNumClusters;
            if (!inNewCluster && !readCluster(cluster, buf_)) {
                    return -1;
            }

            uint32_t copySize;
            if (cluster == startCluster && context->offset != 0) {
                uint32_t offsetInCluster = context->offset % getClusterSizeByte();
                copySize = restSizeToWrite > getClusterSizeByte() - offsetInCluster ? getClusterSizeByte() - offsetInCluster: restSizeToWrite;
                memcpy(buf_ + offsetInCluster, context->memory->Data + sizeWritten, copySize);
            } else {
                copySize = restSizeToWrite > getClusterSizeByte() ? getClusterSizeByte() : restSizeToWrite;
                memcpy(buf_, context->memory->Data + sizeWritten, copySize);
            }
            sizeWritten += copySize;
            if (!writeCluster(cluster, buf_)) {
                monapi_warn("write cluster failed cluster=%x\n", cluster);
                return -1;
            }
        }

        if (flushDirtyFat() != MONA_SUCCESS) {
            return -1;
        }
        return context->size;
    }

    int tryCreateNewEntryInCluster(Vnode* dir, const std::string&file, uint32_t cluster)
    {
        Directory* d = (Directory*)dir->fnode;
        uint8_t buf[getClusterSizeByte()];
        if (!readCluster(cluster, buf)) {
            return M_READ_ERROR;
        }

        uint32_t index = 0;
        for (struct de* entry = (struct de*)buf; (uint8_t*)entry < (uint8_t*)(&buf[getClusterSizeByte()]); entry++, index++) {
            if (entry->name[0] == 0x00 || entry->name[0] == FREE_ENTRY) {
                File* fileEntry = new File(file, 0, 0, cluster, index);
                d->addChild(fileEntry);
                fileEntry->setParent(d);
                setupNewEntry(entry, file);
                if (writeCluster(cluster, buf)) {
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
        if (dev_.write(dirtyFatSector, dirtyFat, SECTOR_SIZE) != M_OK) {
            monapi_warn("failed to update FAT");
            return MONA_FAILURE;
        } else {
            return MONA_SUCCESS;
        }
    }

    int flushDirtyFat()
    {
        for (std::map<uint32_t, uint32_t>::const_iterator it = dirtyFat_.begin(); it != dirtyFat_.end(); ++it) {

            if (flushFat(it->first) != MONA_SUCCESS) {
                dirtyFat_.clear();
                return MONA_FAILURE;
            }
        }
        dirtyFat_.clear();
        return MONA_SUCCESS;
    }

    void updateFatNoFlush(uint32_t index, uint32_t cluster)
    {
        fat_[index] = cluster;
        dirtyFat_.insert(std::pair<uint32_t, uint32_t>(index, index));
    }

    virtual int create(Vnode* dir, const std::string& file)
    {
        ASSERT(dir->type == Vnode::DIRECTORY);
        ASSERT(file.size() <= 11);
        Directory* d = (Directory*)dir->fnode;
        uint32_t cluster = getLastCluster(d);
        int ret = tryCreateNewEntryInCluster(dir, file, cluster);
        if (ret == M_OK) {
            return MONA_SUCCESS;
        } else if (ret == M_NO_SPACE) {
            uint32_t newCluster = allocateCluster();
            if (isEndOfCluster(newCluster)) {
                return MONA_FAILURE;
            }

            if (tryCreateNewEntryInCluster(dir, file, newCluster) != M_OK) {
                return MONA_FAILURE;
            }

            updateFatNoFlush(cluster, newCluster);
            updateFatNoFlush(newCluster, END_OF_CLUSTER);

            if (flushDirtyFat() != MONA_SUCCESS) {
                return MONA_FAILURE;
            }
            return MONA_SUCCESS;
        }
    }
    virtual int readdir(Vnode* directory, monapi_cmemoryinfo** entries)
    {
        typedef std::vector<monapi_directoryinfo> DirInfos;
        DirInfos dirInfos;
        ASSERT(directory->type == Vnode::DIRECTORY);
        Directory* dir = (Directory*)directory->fnode;

        for (Files::const_iterator it = dir->getChildlen().begin(); it != dir->getChildlen().end(); ++it) {
            monapi_directoryinfo di;
            strcpy(di.name, (*it)->getName().c_str());
            di.size = (*it)->getSize();
            dirInfos.push_back(di);
        }

        monapi_cmemoryinfo* ret = monapi_cmemoryinfo_new();

        int size = dirInfos.size();
        if (monapi_cmemoryinfo_create(ret, sizeof(int) + size * sizeof(monapi_directoryinfo), MONAPI_FALSE, true) != M_OK) {
            monapi_cmemoryinfo_delete(ret);
            return MONA_ERROR_MEMORY_NOT_ENOUGH;
        }

        memcpy(ret->Data, &size, sizeof(int));
        monapi_directoryinfo* p = (monapi_directoryinfo*)&ret->Data[sizeof(int)];
        *entries = ret;
        for (DirInfos::const_iterator it = dirInfos.begin(); it != dirInfos.end(); ++it)
        {
            memcpy(p, &(*it), sizeof(monapi_directoryinfo));
            p++;
        }

        return MONA_SUCCESS;
    }

    virtual int close(Vnode* vnode)
    {
        return MONA_SUCCESS;
    }

    virtual int truncate(Vnode* vnode)
    {
        ASSERT(vnode->type == Vnode::REGULAR);
        File* entry = (File*)vnode->fnode;
        if (entry->getSize() == 0) {
            return MONA_SUCCESS;
        }
        if (!readCluster(entry->getClusterInParent(), buf_)) {
            return MONA_FAILURE;
        }
        struct de* theEntry = ((struct de*)buf_) + entry->getIndexInParentCluster();
        setEntry(theEntry, 0, 0);

        if (!writeCluster(entry->getClusterInParent(), buf_)) {
            return M_WRITE_ERROR;
        }

        if (freeClusters(entry) != MONA_SUCCESS) {
            return MONA_FAILURE;
        }
        entry->setStartCluster(0);
        entry->setSize(0);
        return MONA_SUCCESS;
    }
    virtual int delete_file(Vnode* vnode)
    {
        if(vnode == root_) {
            return MONA_FAILURE; // root is undeletable
        }
        File* entry = (File*)vnode->fnode;
        if (!readCluster(entry->getClusterInParent(), buf_)) {
            return MONA_FAILURE;
        }
        struct de* theEntry = ((struct de*)buf_) + entry->getIndexInParentCluster();
        theEntry->name[0] = FREE_ENTRY;
        if (!writeCluster(entry->getClusterInParent(), buf_)) {
            return M_WRITE_ERROR;
        }
        if (freeClusters(entry) != MONA_SUCCESS) {
            return MONA_FAILURE;
        }
        ((Directory*)(entry->getParent()))->removeChild(entry);
        destroyVnode(vnode);
        return MONA_SUCCESS;
    }
    virtual int stat(Vnode* vnode, Stat* st)
    {
        File* entry = (File*)vnode->fnode;
        st->size = entry->getSize();
        return MONA_SUCCESS;
    }

    virtual void destroyVnode(Vnode* vnode)
    {
        File* entry = (File*)vnode->fnode;
        delete entry;
        delete vnode;
    }


    FatFileSystem(VnodeManager& vnodeManager, IStorageDevice& dev) :
        vnodeManager_(vnodeManager),
        dev_(dev),
        root_(vnodeManager.alloc()),
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

    Vnode* getRoot() const
    {
        return root_.get();
    }

    // Public for testability
    uint16_t getBytesPerSector() const
    {
        return little2host16(bsbpb_->bps);
    }


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
            indexInParentCluster_(indexInParentCluster)
        {
        }

        virtual ~File() {}

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

    private:
        const std::string name_;
        uintptr_t size_;
        uintptr_t startCluster_;
        File* parent_;
        uint32_t clusterInParent_;
        uint32_t indexInParentCluster_;
    };

    typedef std::vector<File*> Files;

    class Directory : public File
    {
    public:
        Directory(const std::string& name, uintptr_t startCluster, const Files& childlen, uint32_t clusterInParent, uint32_t indexInParentCluster) : File(name, 0, startCluster, clusterInParent, indexInParentCluster)
        {
            childlen_.resize(childlen.size());
            std::copy(childlen.begin(), childlen.end(), childlen_.begin());
        }

        virtual ~Directory()
        {
            for (Files::const_iterator it = childlen_.begin(); it != childlen_.end(); ++it) {
                delete *it;
            }
        }

        void addChild(File* entry)
        {
            childlen_.push_back(entry);
        }

        Files& getChildlen()
        {
            return childlen_;
        }

        void removeChild(File* entry)
        {
            childlen_.erase(std::remove(childlen_.begin(), childlen_.end(), entry), childlen_.end());
        }
    private:
        Files childlen_;
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
        for (int i = 0; i < index; i++, cluster = fat_[cluster]) {
            ASSERT(!isEndOfCluster(cluster));
        }
        return cluster;
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
        if (dev_.write(absoluteCluster * getSectorsPerCluster(), buf, SECTOR_SIZE * getSectorsPerCluster()) != M_OK) {
            return false;
        }
        return true;
    }

    bool readDirectory(uint32_t startCluster, Files& childlen)
    {
        uint8_t buf[getClusterSizeByte()];
        for (uint32_t cluster = startCluster; !isEndOfCluster(cluster); cluster = fat_[cluster]) {
            if (!readCluster(cluster, buf) != M_OK) {
                return false;
            }

            uint32_t index = 0;
            for (struct de* entry = (struct de*)buf; (uint8_t*)entry < (uint8_t*)(&buf[getClusterSizeByte()]); entry++, index++) {
                if (entry->name[0] == 0x00) {
                    return true;
                }
                // Long file name
                if (entry->attr == 0x0f) {
                    continue;
                }
                if (entry->name[0] == 0x2e || entry->name[0] == FREE_ENTRY) {
                    continue;
                }
                std::string filename;
                for (int i = 0; i < 8; i++) {
                    if (entry->name[i] == ' ') {
                        break;
                    }
                    filename += entry->name[i];
                }
                std::string ext = std::string((char*)entry->ext, 3);
                if (ext != "   ") {
                    filename += '.';
                    filename += ext;
                }

                logprintf("%s:", filename.c_str());
                logprintf("<%d>", little2host16(entry->clus));
                logprintf("<%x>\n", fat_[little2host16(entry->clus)]);
                File* target = NULL;
                if (entry->attr & ATTR_SUBDIR) {
                    Files childlen;
                    // For now, we read all directories recursively.
                    // This may cause slower initialization.
                    if (!readDirectory(little2host16(entry->clus), childlen)) {
                        return false;
                    }
                    target = new Directory(filename, little2host16(entry->clus), childlen, cluster, index);
                } else {
                    target = new File(filename, little2host32(entry->size), little2host16(entry->clus), cluster, index);
                }
                childlen.push_back(target);
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

        root_->fnode = new Directory("", getRootDirectoryCluster(), childlen, 0, 0);
        root_->fs = this;
        root_->type = Vnode::DIRECTORY;

        for (Files::iterator it = childlen.begin(); it != childlen.end(); ++it) {
            (*it)->setParent((Directory*)root_->fnode);
        }

        return true;
    }

    uint32_t getClusterSizeByte() const
    {
        return getSectorsPerCluster() * SECTOR_SIZE;
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
        FREE_ENTRY = 0xe5,
        SECTOR_SIZE = 512,
        ATTR_SUBDIR = 0x10,
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

    enum {
        END_OF_CLUSTER = 0x0FFFFFF8
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

    void setupNewEntry(struct de* entry, const std::string filename)
    {
        std::vector<std::string> nameAndExt = split(filename, '.');
        ASSERT(nameAndExt.size() == 2);
        memset(entry->name, ' ', 8);
        ASSERT(nameAndExt[0].size() <= 8);
        memcpy(entry->name, nameAndExt[0].c_str(), nameAndExt[0].size());
        memset(entry->ext, ' ', 3);
        ASSERT(nameAndExt[1].size() <= 3);
        memcpy(entry->ext, nameAndExt[1].c_str(), nameAndExt[1].size());
        setEntry(entry, 0, 0);
    }

    intptr_t updateParentCluster(File* entry)
    {
        if (!readCluster(entry->getClusterInParent(), buf_)) {
            return M_READ_ERROR;
        }

        struct de* theEntry = ((struct de*)buf_) + entry->getIndexInParentCluster();
        setEntry(theEntry, entry->getStartCluster(), entry->getSize());
        if (!writeCluster(entry->getClusterInParent(), buf_)) {
            return M_WRITE_ERROR;
        }
        return M_OK;
    }

    int expandFileAsNecessary(File* entry, io::Context* context)
    {
        uint32_t tailOffset = context->offset + context->size;
        uint32_t currentNumClusters = sizeToNumClusters(entry->getSize());
        uint32_t newNumClusters = tailOffset > entry->getSize() ? sizeToNumClusters(tailOffset) : currentNumClusters;
        if (newNumClusters == currentNumClusters) {
            return M_OK;
        }

        Clusters clusters;
        if (!allocateClusters(clusters, newNumClusters - currentNumClusters)) {
            return M_NO_SPACE;
        }
        ASSERT(clusters.size() > 0);

        bool isParentClusterDirty = false;
        if (tailOffset > entry->getSize()) {
            entry->setSize(tailOffset);
            isParentClusterDirty = true;
        }

        if (entry->getSize() == 0) {
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
        if (flushDirtyFat() != MONA_SUCCESS) {
            return MONA_FAILURE;
        }
        return MONA_SUCCESS;
    }


    uint8_t bootParameters_[SECTOR_SIZE];
    struct bsbpb* bsbpb_;
    struct bsxbpb* bsxbpb_;
    VnodeManager& vnodeManager_;
    IStorageDevice& dev_;
    MonAPI::scoped_ptr<Vnode> root_;
    uint32_t* fat_;
    std::map<uint32_t, uint32_t> dirtyFat_;
    uint8_t* buf_;
};

#endif // __FAT_H__
