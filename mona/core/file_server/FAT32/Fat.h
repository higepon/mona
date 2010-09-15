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
        }
        context->resultSize = sizeToRead;
        context->offset += sizeToRead;
        return MONA_SUCCESS;
    }
    virtual int write(Vnode* file, struct io::Context* context)                           {}
    virtual int create(Vnode* dir, const std::string& file)                               {}
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
        root_(vnodeManager.alloc())
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
        Entry(const std::string& name, uintptr_t size, uintptr_t startCluster) : name_(name), size_(size), startCluster_(startCluster)
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

        uintptr_t getStartCluster() const
        {
            return startCluster_;
        }

    private:
        const std::string name_;
        uintptr_t size_;
        uintptr_t startCluster_;
    };

    typedef std::vector<Entry*> Entries;

    class Directory : public Entry
    {
    public:
        Directory(const std::string& name, uintptr_t startCluster, const Entries& childlen) : Entry(name, 0, startCluster)
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

        Entries& getChildlen()
        {
            return childlen_;
        }
    private:
        Entries childlen_;
    };

private:

    uint32_t traceClusterChain(uint32_t startCluster, uint32_t count)
    {
        uint32_t cluster = startCluster;
        for (int i = 0; i < count; i++, cluster = fat_[cluster]) {
            ASSERT(cluster < END_OF_CLUSTER);
        }
        return cluster;
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
        MonAPI::scoped_ptr<uint32_t> buf(new uint32_t[fatSizeByte / sizeof(uint32_t)]);

        if (dev_.read(fatStartSector, buf.get(), fatSizeByte) != M_OK) {
            return false;
        }
        for (int i = 0; i < fatSizeByte / sizeof(uint32_t); i++) {
            fat_.push_back(buf.get()[i]);
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

    bool readDirectory(uint32_t startCluster, Entries& childlen)
    {
        uint8_t buf[getClusterSizeByte()];
        for (uint32_t cluster = startCluster; cluster < END_OF_CLUSTER; cluster = fat_[cluster]) {
            if (!readCluster(cluster, buf) != M_OK) {
                return false;
            }

            for (struct de* entry = (struct de*)buf; (uint8_t*)entry < (uint8_t*)(&buf[getClusterSizeByte()]); entry++) {
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
                childlen.push_back(new Entry(filename, little2host32(entry->size), little2host16(entry->clus)));
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
        root_->fnode = new Directory("", getRootDirectoryCluster(), childlen);
        root_->fs = this;
        root_->type = Vnode::DIRECTORY;
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

    uint8_t bootParameters_[SECTOR_SIZE];
    struct bsbpb* bsbpb_;
    struct bsxbpb* bsxbpb_;
    VnodeManager& vnodeManager_;
    IStorageDevice& dev_;
    MonAPI::scoped_ptr<Vnode> root_;
    std::vector<uint32_t> fat_;
};

#endif // __FAT_H__
