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
// create date
// create duplicate check

// write
//   reserve all cluster
//   isendofclsuter

// fat class
// share the buffer
class FatFileSystem : public FileSystem
{
public:
    virtual int initialize() {}
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
        Entries& childlen = dir->getChildlen();
        for (Entries::const_iterator it = childlen.begin(); it != childlen.end(); ++it) {
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

    virtual int open(Vnode* file, intptr_t mode)                                          {}
    virtual int read(Vnode* file, struct io::Context* context)
    {
        if (file->type != Vnode::REGULAR) {
            return MONA_FAILURE;
        }
        Entry* e = (Entry*)file->fnode;
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
        logprintf("read sizeToRead=%d\n", sizeToRead);
        // Use immediate map for performance reason.
        if (monapi_cmemoryinfo_create(context->memory, sizeToRead, MONAPI_FALSE, true) != M_OK) {
            monapi_cmemoryinfo_delete(context->memory);
            return MONA_ERROR_MEMORY_NOT_ENOUGH;
        }

        uint32_t skipClusterCount = offset / getClusterSizeByte();
        uint32_t startCluster = traceClusterChain(e->getStartCluster(), skipClusterCount);

        uint8_t buf[getClusterSizeByte()];
        uint32_t sizeRead = 0;
        uint32_t offsetInBuf = context->offset - getClusterSizeByte() * skipClusterCount;
        for (uint32_t cluster = startCluster; ; cluster = fat_[cluster]) {
            ASSERT(cluster < END_OF_CLUSTER);
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
            logprintf("cluster %x => %x\n", cluster, fat_[cluster]);
        }
        context->resultSize = sizeToRead;
        context->offset += sizeToRead;
        return MONA_SUCCESS;
    }

    uint32_t sizeToNumClusters(uint32_t sizeByte) const
    {
        return (sizeByte + getClusterSizeByte() - 1) / getClusterSizeByte();
    }

    virtual int write(Vnode* file, struct io::Context* context)
    {
        if (file->type != Vnode::REGULAR) {
            return MONA_FAILURE;
        }
        ASSERT(context->memory);
        Entry* entry = (Entry*)file->fnode;
        if (entry->getSize() == 0) {
            return writeToEmptyFile(entry, context);
        } else {
            return writeToNonEmptyFile(entry, context);
        }
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
            if (entry->name[0] == 0x00) {
                Entry* fileEntry = new Entry(file, 0, 0, cluster, index);
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

        logprintf("dirty fat sector = %d", dirtyFatSector);

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
            uint32_t newCluster = findEmptyCluster();
            if (newCluster == END_OF_CLUSTER) {
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
    virtual int truncate(Vnode* file)                                                     {}
    virtual int readdir(Vnode* directory, monapi_cmemoryinfo** entries)                   {}
    virtual int close(Vnode* file)                                                        {}
    virtual int delete_file(Vnode* file)                                                  {}
    virtual int stat(Vnode* file, Stat* st)                                               {}
    virtual void destroyVnode(Vnode* vnode)
    {
    }


    FatFileSystem(VnodeManager& vnodeManager, IStorageDevice& dev) :
        vnodeManager_(vnodeManager),
        dev_(dev),
        root_(vnodeManager.alloc()),
        fat_(NULL)
    {
        ASSERT(root_.get());

        if (!readBootParameters()) {
            monapi_fatal("can't read boot parameter block");
        }

        if (!readFat()) {
            monapi_fatal("can't read file allocation table");
        }

        if (!readAndSetupRootDirectory()) {
            monapi_fatal("can't read root directory");
        }

    }

    ~FatFileSystem()
    {
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
    class Entry
    {
    public:
        Entry(const std::string& name, uintptr_t size, uint32_t startCluster, uint32_t clusterInParent, uint32_t indexInParentCluster) :
            name_(name),
            size_(size),
            startCluster_(startCluster),
            parent_(NULL),
            clusterInParent_(clusterInParent),
            indexInParentCluster_(indexInParentCluster)
        {
        }

        virtual ~Entry() {}

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

        Entry* getParent() const
        {
            return parent_;
        }

        void setParent(Entry* parent)
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
        Entry* parent_;
        uint32_t clusterInParent_;
        uint32_t indexInParentCluster_;
    };

    typedef std::vector<Entry*> Entries;

    class Directory : public Entry
    {
    public:
        Directory(const std::string& name, uintptr_t startCluster, const Entries& childlen, uint32_t clusterInParent, uint32_t indexInParentCluster) : Entry(name, 0, startCluster, clusterInParent, indexInParentCluster)
        {
            childlen_.resize(childlen.size());
            std::copy(childlen.begin(), childlen.end(), childlen_.begin());
        }

        virtual ~Directory()
        {
            for (Entries::const_iterator it = childlen_.begin(); it != childlen_.end(); ++it) {
                delete *it;
            }
        }

        void addChild(Entry* entry)
        {
            childlen_.push_back(entry);
        }

        Entries& getChildlen()
        {
            return childlen_;
        }
    private:
        Entries childlen_;
    };

private:

    typedef std::vector<uint32_t> Clusters;

    bool findEmptyClusters(Clusters& clusters, uint32_t numClusters)
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


    uint32_t findEmptyCluster()
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

    uint32_t traceClusterChain(uint32_t startCluster, uint32_t count)
    {
        uint32_t cluster = startCluster;
        for (int i = 0; i < count; i++, cluster = fat_[cluster]) {
            ASSERT(cluster < END_OF_CLUSTER);
        }
        return cluster;
    }

    uint32_t getLastCluster(Entry* entry)
    {
        for (uint32_t cluster = entry->getStartCluster(); ; cluster = fat_[cluster]) {
            if (fat_[cluster] >= END_OF_CLUSTER) {
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

    bool readDirectory(uint32_t startCluster, Entries& childlen)
    {
        uint8_t buf[getClusterSizeByte()];
        for (uint32_t cluster = startCluster; cluster < END_OF_CLUSTER; cluster = fat_[cluster]) {
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
                std::string filename;
                for (int i = 0; i < 8; i++) {
                    if (entry->name[i] == ' ') {
                        break;
                    }
                    filename += entry->name[i];
                }
                filename += '.';
                filename += std::string((char*)entry->ext, 3);
                logprintf("%s:", filename.c_str());
                logprintf("<%d>", little2host16(entry->clus));
                logprintf("<%x>\n", fat_[little2host16(entry->clus)]);
                childlen.push_back(new Entry(filename, little2host32(entry->size), little2host16(entry->clus), cluster, index));
            }
        }
        return true;
    }

    bool readAndSetupRootDirectory()
    {
        Entries childlen;

        if (!readDirectory(getRootDirectoryCluster(), childlen)) {
            return false;
        }

        root_->fnode = new Directory("", getRootDirectoryCluster(), childlen, 0, 0);
        root_->fs = this;
        root_->type = Vnode::DIRECTORY;

        for (Entries::iterator it = childlen.begin(); it != childlen.end(); ++it) {
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
        SECTOR_SIZE = 512
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
        END_OF_CLUSTER = 0x0FFFFFF8,
        forbiddend_comma
    };

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
        *((uint32_t*)entry->size) = 0;
        *((uint16_t*)entry->clus) = 0;
    }

    intptr_t updateParentCluster(Entry* entry)
    {
        uint8_t buf[getClusterSizeByte()];
        if (!readCluster(entry->getClusterInParent(), buf)) {
            return M_READ_ERROR;
        }

        struct de* theEntry = ((struct de*)buf) + entry->getIndexInParentCluster();
        *((uint32_t*)theEntry->size) = entry->getSize();
        *((uint16_t*)theEntry->clus) = entry->getStartCluster();
        if (!writeCluster(entry->getClusterInParent(), buf)) {
            return M_WRITE_ERROR;
        }
        return M_OK;
    }

    int writeToEmptyFile(Entry* entry, struct io::Context* context)
    {
        ASSERT(entry->getSize() == 0);
        ASSERT(context->offset == 0);
        Clusters clusters;
        if (!findEmptyClusters(clusters, sizeToNumClusters(context->size))) {
            return -1;
        }

        entry->setStartCluster(clusters[0]);
        entry->setSize(context->size);
        if (updateParentCluster(entry) != M_OK) {
            return -1;
        }

        uint8_t buf[getClusterSizeByte()];
        uint32_t sizeToWrite = context->size;
        for (uint32_t i = 0; i < clusters.size(); i++) {
            uint32_t cluster = clusters[i];
            uint32_t copySize = sizeToWrite > getClusterSizeByte() ? getClusterSizeByte() : sizeToWrite;
            memcpy(buf, context->memory->Data + getClusterSizeByte() * i, copySize);
            if (!writeCluster(cluster, buf)) {
                return -1;
            }
            sizeToWrite -= copySize;
        }

        for (uint32_t i = 0; i < clusters.size(); i++) {
            updateFatNoFlush(clusters[i], (i == clusters.size() - 1) ? END_OF_CLUSTER : clusters[i + 1]);
        }
        if (flushDirtyFat() != MONA_SUCCESS) {
            return -1;
        }
        return context->size;
    }

    int writeToNonEmptyFile(Entry* entry, struct io::Context* context)
    {
        uint32_t endOfWrite = context->offset + context->size;
        uint32_t currentNumClusters = sizeToNumClusters(entry->getSize());
        uint32_t newNumClusters = endOfWrite > entry->getSize() ? sizeToNumClusters(endOfWrite) : currentNumClusters;
        logprintf("newNumClusters = %d currentNumClusters=%d\n", newNumClusters, currentNumClusters);
        Clusters clusters;
        if (newNumClusters > currentNumClusters) {
            if (!findEmptyClusters(clusters, newNumClusters - currentNumClusters)) {
                return -1;
            }
        }
        logprintf("clusters.size=%d\n", clusters.size());
        uint32_t clusterOffset = sizeToNumClusters(context->offset) - 1;
        uint32_t startCluster = traceClusterChain(entry->getStartCluster(), clusterOffset);
        ASSERT(startCluster != END_OF_CLUSTER);
        uint8_t buf[getClusterSizeByte()];

        uint32_t offsetInCluster = context->offset % getClusterSizeByte();
        uint32_t sizeToWrite = context->size;
        uint32_t sizeWritten = 0;
        logprintf("START\n");
        int newClusterIndex = 0;
        for (uint32_t cluster = startCluster, clusterIndex = clusterOffset; ; clusterIndex++) {
            logprintf("writing\n");
            uint32_t restSizeToWrite = sizeToWrite - sizeWritten;

            bool inNewCluster = clusterIndex >= currentNumClusters;
            if (!inNewCluster) {
                if (!readCluster(cluster, buf)) {
                    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
                    return -1;
                }
            }

            uint32_t copySize = restSizeToWrite > getClusterSizeByte() - offsetInCluster ? getClusterSizeByte() - offsetInCluster: restSizeToWrite;
            memcpy(buf + offsetInCluster, context->memory->Data + sizeWritten, copySize);

            if (!writeCluster(cluster, buf)) {
                monapi_warn("write cluster failed cluster=%x\n", cluster);
                return -1;
            }
            offsetInCluster = 0;
            sizeWritten += copySize;
            if (sizeWritten == sizeToWrite) {
                if (inNewCluster) {
                    updateFatNoFlush(cluster, END_OF_CLUSTER);
                }
                break;
            }

            // ToDo:fat END_OF_CLUSTER when over.

            if (fat_[cluster] == END_OF_CLUSTER) {
                logprintf("newClusterIndex=%d cluster.size=%d\n", newClusterIndex, clusters.size());
                ASSERT(newClusterIndex < clusters.size());
                uint32_t newCluster = clusters[newClusterIndex++];
                updateFatNoFlush(cluster, newCluster);
                cluster = newCluster;
            } else {
                cluster = fat_[cluster];
            }
        }

        if (endOfWrite > entry->getSize()) {
            entry->setSize(endOfWrite);
            if (updateParentCluster(entry) != M_OK) {
                return -1;
            }
        }
        flushDirtyFat();
        return context->size;
    }

    uint8_t bootParameters_[SECTOR_SIZE];
    struct bsbpb* bsbpb_;
    struct bsxbpb* bsxbpb_;
    VnodeManager& vnodeManager_;
    IStorageDevice& dev_;
    MonAPI::scoped_ptr<Vnode> root_;
    uint32_t* fat_;
    std::map<uint32_t, uint32_t> dirtyFat_;
};

#endif // __FAT_H__
